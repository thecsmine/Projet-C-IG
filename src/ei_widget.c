#include <assert.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_types.h"
#include "ei_utils.h"
#include "ei_widget.h"
#include "ei_widgetclass.h"

#include "ei_draw_utils.h"
#include "ei_widget_utils.h"
#include "ei_application_utils.h"

/**
 * @brief	Creates a new instance of a widget of some particular class, as a descendant of
 *		an existing widget.
 *
 *		The widget is not displayed on screen until it is managed by a geometry manager.
 *		When no more needed, the widget must be released by calling \ref ei_widget_destroy.
 *
 * @param	class_name	The name of the class of the widget that is to be created.
 * @param	parent 		A pointer to the parent widget. Can not be NULL.
 * @param	user_data	A pointer provided by the programmer for private use. May be NULL.
 * @param	destructor	A pointer to a function to call before destroying a widget structure. May be NULL.
 *
 * @return			The newly created widget, or NULL if there was an error.
 */
ei_widget_t *ei_widget_create(ei_widgetclass_name_t class_name,
			      ei_widget_t *parent,
			      void *user_data,
			      ei_widget_destructor_t destructor) {
	ei_widget_t *widget;
	ei_widgetclass_t *wclass = ei_widgetclass_from_name(class_name);

	if (wclass != NULL) {
		widget = wclass->allocfunc();
		widget->wclass = wclass;
		widget->wclass->setdefaultsfunc(widget);
	} else {
		return NULL; // class_name not recognized
	}

	// Initialisation des attributs communs à tous les widgets
	widget->pick_id = ei_get_widget_id();
	widget->pick_color = malloc(sizeof(ei_color_t));
	*widget->pick_color = pixel_to_rgba(ei_get_pick_surface(), widget->pick_id);
	widget->user_data = user_data;
	widget->destructor = destructor;

	// Widget Hierachy Management
	widget->parent = parent;
	widget->next_sibling = NULL;
	widget->children_head = NULL;
	widget->children_tail = NULL;

	// Add widget as last child of parent
	if (widget->parent != NULL) {
		if (widget->parent->children_tail != NULL) {
			assert((widget->parent->children_tail->next_sibling == NULL));
			widget->parent->children_tail->next_sibling = widget;
			widget->parent->children_tail = widget;
		} else {
			assert((widget->parent->children_head == NULL));
			widget->parent->children_head = widget;
			widget->parent->children_tail = widget;
		}
	}

	// Widget geometry
	widget->placer_params = NULL;
	widget->requested_size = ei_size_zero();
	widget->screen_location = ei_rect_zero();
	widget->content_rect = &(widget->screen_location);

	return widget;
}


/**
 * @brief	Destroys a widget. Calls its destructor if it was provided.
 * 		Removes the widget from the screen if it is currently managed by the placer.
 * 		Destroys all its descendants.
 *
 * @param	widget		The widget that is to be destroyed.
 */
void ei_widget_destroy(ei_widget_t *widget) {
	// Call destructor if provided
	if (widget->destructor != NULL) {
		widget->destructor(widget);
	}

	// Removes from screen if managed by placer
	if (widget->placer_params != NULL) {
		ei_app_invalidate_rect(&widget->screen_location); // Includes content_rect
		ei_placer_forget(widget);
	}

	// Destroys its descendants
	ei_widget_t *ptr = widget->children_head;
	ei_widget_t *old = widget->children_head;
	while (ptr != NULL) {
		ptr = ptr->next_sibling;
		ei_widget_destroy_child(old);
		old = ptr;
	}

	// Link correctly between siblings and parent (parcours linéaire)
	// Link between siblings
	if (widget->parent != NULL) {
		ptr = widget->parent->children_head;
		if (ptr == widget) {
			widget->parent->children_head = widget->next_sibling;
			if (widget->parent->children_tail == widget) {
				widget->parent->children_tail = widget->next_sibling;
			}
		} else {
			while (ptr->next_sibling != widget) {
				ptr = ptr->next_sibling;
			}
			ptr->next_sibling = widget->next_sibling;
			if (widget->next_sibling == NULL) {
				widget->parent->children_tail = ptr;
			}
		}
	}

	// Frees memory
	widget->wclass->releasefunc(widget);
	free(widget);
}


/**
 * @brief	Returns the widget that is at a given location on screen.
 *
 * @param	where		The location on screen, expressed in the root window coordinates.
 *
 * @return			The top-most widget at this location, or NULL if there is no widget
 *				at this location (except for the root widget).
 */
ei_widget_t *ei_widget_pick(ei_point_t *where) {
	ei_surface_t pick_surface = ei_get_pick_surface();
	hw_surface_lock(pick_surface);
	uint32_t *pixel_ptr = (uint32_t *) hw_surface_get_buffer(pick_surface);
	pixel_ptr += where->x + where->y * hw_surface_get_size(pick_surface).width;
	hw_surface_unlock(pick_surface);
	return ei_find_widget_by_id(*pixel_ptr);
}


void ei_frame_configure(ei_widget_t *widget,
			ei_size_t *requested_size,
			const ei_color_t *color,
			int *border_width,
			ei_relief_t *relief,
			char **text,
			ei_font_t *text_font,
			ei_color_t *text_color,
			ei_anchor_t *text_anchor,
			ei_surface_t *img,
			ei_rect_t **img_rect,
			ei_anchor_t *img_anchor) {
	// Polymorphisme
	ei_frame_t *frame = (ei_frame_t *) widget;

	if (color != NULL) {
		frame->color = *color;
	}
	if (border_width != NULL) {
		frame->border_width = *border_width;
	}
	if (relief != NULL) {
		frame->relief = *relief;
	}
	if (text != NULL) {
		frame->text = *text;
		frame->img = NULL;
		frame->img_rect = NULL;
	}
	if (text_font != NULL) {
		frame->text_font = *text_font;
	}
	if (text_color != NULL) {
		frame->text_color = *text_color;
	}
	if (text_anchor != NULL) {
		frame->text_anchor = *text_anchor;
	}
	if (img != NULL) {
		frame->img = img;
		frame->text = NULL;
	}
	if (img_rect != NULL) {
		frame->img_rect = *img_rect;
	}
	if (img_anchor != NULL) {
		frame->img_anchor = *img_anchor;
	}
	if (requested_size != NULL) { // last to configure
		widget->requested_size = *requested_size;
		frame->requested_bool = EI_TRUE;
	} else if (!frame->requested_bool) {
		widget->requested_size = ei_widget_natural_size(frame->border_width, frame->text, frame->text_font,
								frame->img_rect);
	}
}


void ei_button_configure(ei_widget_t *widget,
			 ei_size_t *requested_size,
			 const ei_color_t *color,
			 int *border_width,
			 int *corner_radius,
			 ei_relief_t *relief,
			 char **text,
			 ei_font_t *text_font,
			 ei_color_t *text_color,
			 ei_anchor_t *text_anchor,
			 ei_surface_t *img,
			 ei_rect_t **img_rect,
			 ei_anchor_t *img_anchor,
			 ei_callback_t *callback,
			 void **user_param) {
	// Polymorphisme
	ei_button_t *button = (ei_button_t *) widget;

	if (color != NULL) {
		button->color = *color;
	}
	if (border_width != NULL) {
		button->border_width = *border_width;
	}
	if (corner_radius != NULL) {
		button->corner_radius = *corner_radius;
	}
	if (relief != NULL) {
		button->relief = *relief;
	}
	if (text != NULL) {
		button->text = *text;
		button->img = NULL;
		button->img_rect = NULL;
	}
	if (text_font != NULL) {
		button->text_font = *text_font;
	}
	if (text_color != NULL) {
		button->text_color = *text_color;
	}
	if (text_anchor != NULL) {
		button->text_anchor = *text_anchor;
	}
	if (img != NULL) {
		button->img = img;
		button->text = NULL;
	}
	if (img_rect != NULL) {
		button->img_rect = *img_rect;
	}
	if (img_anchor != NULL) {
		button->img_anchor = *img_anchor;
	}
	if (callback != NULL) {
		button->callback = *callback;
	}
	if (user_param != NULL) {
		button->user_param = *user_param;
	}
	if (requested_size != NULL) { // last to configure
		widget->requested_size = *requested_size;
		button->requested_bool = EI_TRUE;
	} else if (!button->requested_bool) {
		widget->requested_size = ei_widget_natural_size(button->border_width, button->text, button->text_font,
								button->img_rect);
	}
}


void ei_toplevel_configure(ei_widget_t *widget,
			   ei_size_t *requested_size,
			   ei_color_t *color,
			   int *border_width,
			   char **title,
			   ei_bool_t *closable,
			   ei_axis_set_t *resizable,
			   ei_size_t **min_size) {
	// Polymorphisme
	ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;

	if (requested_size != NULL) {
		widget->requested_size = *requested_size;
	}
	if (color != NULL) {
		toplevel->color = *color;
	}
	if (border_width != NULL) {
		toplevel->border_width = *border_width;
	}
	if (title != NULL) {
		toplevel->title = *title;
	}
	if (closable != NULL) {
		toplevel->closable = *closable;
	}
	if (resizable != NULL) {
		toplevel->resizable = *resizable;
	}
	if (min_size != NULL) {
		if (*min_size != NULL) {
			toplevel->min_size = ei_size(160, 120);
		} else {
			toplevel->min_size = **min_size;
		}
	}
}
