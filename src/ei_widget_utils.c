#include <stdlib.h>
#include <string.h>

#include "ei_application.h"
#include "ei_event.h"
#include "ei_placer.h"
#include "ei_types.h"
#include "ei_utils.h"
#include "ei_widget.h"

#include "ei_button.h"
#include "ei_draw_utils.h"
#include "ei_widget_utils.h"
#include "ei_application_utils.h"

/** Global variables **/
/**                  **/
uint32_t general_id = 0xff000000;
/**                  **/
/** ---------------- **/

void empty_callback(ei_widget_t *widget, struct ei_event_t *event, void *user_param) {
	return;
}

void ei_widget_destroy_child(ei_widget_t *widget) {
	// Call destructor if provided
	if (widget->destructor != NULL) {
		widget->destructor(widget);
	}

	// Destroys its descendants
	ei_widget_t *ptr = widget->children_head;
	ei_widget_t *old = widget->children_head;
	while (ptr != NULL) {
		ptr = ptr->next_sibling;
		ei_widget_destroy_child(old);
		old = ptr;
	}

	// No need to link correctly between siblings and parent

	// Frees memory
	widget->wclass->releasefunc(widget);
	free(widget);
}

uint32_t ei_get_widget_id(void) {
	general_id += 0x2277cc45;
	return general_id;
}

ei_widget_t *ei_find_widget_by_id(uint32_t id) {
	return search_widget(ei_app_root_widget(), id);
}

ei_widget_t *search_widget(ei_widget_t *widget, uint32_t id) {
	if (widget->pick_id == id) {
		return widget;
	} else {
		if (widget->next_sibling == NULL && widget->children_head == NULL) {
			return NULL;
		} else if (widget->next_sibling != NULL && widget->children_head == NULL) {
			return search_widget(widget->next_sibling, id);
		} else if (widget->next_sibling == NULL && widget->children_head != NULL) {
			return search_widget(widget->children_head, id);
		} else {
			ei_widget_t *wsibling = search_widget(widget->next_sibling, id);
			if (wsibling != NULL) {
				return wsibling;
			}
			ei_widget_t *wchild = search_widget(widget->children_head, id);
			if (wchild != NULL) {
				return wchild;
			}
			return NULL;
		}

	}
}

ei_size_t ei_widget_natural_size(int border_width, char *text, ei_font_t text_font, ei_rect_t *img_rect) {
	ei_size_t requested_size;
	if (text != NULL) {
		hw_text_compute_size(text, text_font, &requested_size.width, &requested_size.height);
	} else if (img_rect != NULL) {
		requested_size = img_rect->size;
	} else {
		requested_size = ei_size_zero();
	}
	requested_size.width += border_width;
	requested_size.height += border_width;
	return requested_size;
}

ei_frame_t ei_init_default_frame(void) {
	ei_frame_t frame;
	// frame.widget is not initialized
	frame.color = ei_default_background_color;
	frame.border_width = 0;
	frame.relief = ei_relief_none;
	frame.text = NULL;
	frame.text_font = ei_default_font;
	frame.text_color = ei_font_default_color;
	frame.text_anchor = ei_anc_center;
	frame.img = NULL;
	frame.img_rect = NULL;
	frame.img_anchor = ei_anc_center;
	frame.requested_bool = EI_FALSE;
	return frame;
}

ei_widgetclass_t ei_init_frame_class(void) {
	ei_widgetclass_t wclass;
	strcpy(wclass.name, "frame");
	wclass.allocfunc = &frame_allocfunc;
	wclass.releasefunc = &frame_releasefunc;
	wclass.drawfunc = &frame_drawfunc;
	wclass.setdefaultsfunc = &frame_setdefaultsfunc;
	wclass.geomnotifyfunc = &frame_geomnotifyfunc;
	wclass.handlefunc = &frame_handlefunc;
	wclass.next = NULL;
	return wclass;
}

ei_widget_t *frame_allocfunc(void) {
	ei_widget_t *widget = malloc(sizeof(ei_frame_t));
	return widget;
}

void frame_releasefunc(ei_widget_t *widget) {
	ei_frame_t *frame = (ei_frame_t *) widget;

	// Free widget fields allocated by library
	ei_placer_forget(widget);
	free(widget->pick_color);
}

void
frame_drawfunc(ei_widget_t *widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t *clipper) {
	ei_frame_t *frame = (ei_frame_t *) widget;
	draw_frame(surface, frame->text, frame->text_font, frame->text_color, clipper, widget->screen_location,
		   frame->color,
		   frame->relief, EI_FALSE);
	draw_frame(pick_surface, NULL, frame->text_font, frame->text_color, clipper, widget->screen_location,
		   *widget->pick_color,
		   ei_relief_none, EI_TRUE);

}

void frame_setdefaultsfunc(ei_widget_t *widget) {
	ei_frame_t *frame = (ei_frame_t *) widget;
	ei_widgetclass_t *wclass = widget->wclass;
	*frame = ei_init_default_frame();
	widget->wclass = wclass;
	widget->requested_size = ei_widget_natural_size(frame->border_width, frame->text, frame->text_font,
							frame->img_rect);
	widget->content_rect = malloc(sizeof(ei_rect_t));
}

void frame_geomnotifyfunc(ei_widget_t *widget, ei_rect_t rect) {
	*(widget->content_rect) = rect;
}

ei_bool_t frame_handlefunc(ei_widget_t *widget, ei_event_t *event) {
	if (is_located_event(*event)) {
		int x_min = widget->screen_location.top_left.x;
		int x_max = widget->screen_location.top_left.x + widget->screen_location.size.width;
		int y_min = widget->screen_location.top_left.y;
		int y_max = widget->screen_location.top_left.y + widget->screen_location.size.height;
		int x_mouse = event->param.mouse.where.x;
		int y_mouse = event->param.mouse.where.y;
		if (x_mouse >= x_min && x_mouse <= x_max && y_mouse >= y_min &&
		    y_mouse <= y_max) {
			return EI_TRUE;
		} else {
			return EI_FALSE;
		}
	} else {
		return EI_FALSE;
	}
}


ei_button_t ei_init_default_button(void) {
	ei_button_t button;
	// button.widget is not initialized
	button.color = ei_default_background_color;
	button.border_width = k_default_button_border_width;
	button.corner_radius = k_default_button_corner_radius;
	button.relief = ei_relief_raised;
	button.text = NULL;
	button.text_font = ei_default_font;
	button.text_color = ei_font_default_color;
	button.text_anchor = ei_anc_center;
	button.img = NULL;
	button.img_rect = NULL;
	button.img_anchor = ei_anc_center;
	button.callback = empty_callback;
	button.user_param = NULL;
	button.requested_bool = EI_FALSE;
	return button;
}

ei_widgetclass_t ei_init_button_class(void) {
	ei_widgetclass_t wclass;
	strcpy(wclass.name, "button");
	wclass.allocfunc = &button_allocfunc;
	wclass.releasefunc = &button_releasefunc;
	wclass.drawfunc = &button_drawfunc;
	wclass.setdefaultsfunc = &button_setdefaultsfunc;
	wclass.geomnotifyfunc = &button_geomnotifyfunc;
	wclass.handlefunc = &button_handlefunc;
	wclass.next = NULL;
	return wclass;
}

ei_widget_t *button_allocfunc(void) {
	ei_widget_t *widget = malloc(sizeof(ei_button_t));
	return widget;
}

void button_releasefunc(ei_widget_t *widget) {
	ei_button_t *button = (ei_button_t *) widget;

	// Free widget fields allocated by library
	ei_placer_forget(widget);
	free(widget->pick_color);
}

void
button_drawfunc(ei_widget_t *widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t *clipper) {
	struct ei_button_t *button = (ei_button_t *) widget;
	draw_button(surface, button->text, button->text_font, button->text_color, clipper, widget->screen_location,
		    button->color, button->corner_radius, button->relief, EI_FALSE);
	draw_button(pick_surface, NULL, button->text_font, button->text_color, clipper, widget->screen_location,
		    *widget->pick_color, button->corner_radius, ei_relief_none, EI_TRUE);
}

void button_setdefaultsfunc(ei_widget_t *widget) {
	ei_button_t *button = (ei_button_t *) widget;
	ei_widgetclass_t *wclass = widget->wclass;
	*button = ei_init_default_button();
	widget->wclass = wclass;
	widget->requested_size = ei_widget_natural_size(button->border_width, button->text, button->text_font,
							button->img_rect);
	widget->content_rect = malloc(sizeof(ei_rect_t));
}

void button_geomnotifyfunc(ei_widget_t *widget, ei_rect_t rect) {
	*(widget->content_rect) = rect;
}

ei_bool_t button_handlefunc(ei_widget_t *widget, ei_event_t *event) {
	if (is_located_event(*event)) {
		struct ei_button_t *button = (ei_button_t *) widget;
		int x_min = widget->screen_location.top_left.x;
		int x_max = widget->screen_location.top_left.x + widget->screen_location.size.width;
		int y_min = widget->screen_location.top_left.y;
		int y_max = widget->screen_location.top_left.y + widget->screen_location.size.height;
		int x_mouse = event->param.mouse.where.x;
		int y_mouse = event->param.mouse.where.y;
		if (x_mouse >= x_min && x_mouse <= x_max && y_mouse >= y_min &&
		    y_mouse <= y_max) {
		        //event à l'intérieur du bouton
			if (event->type == ei_ev_mouse_buttondown || event->type == ei_ev_mouse_move) {
				if (button->relief != ei_relief_sunken) {
				        //affichage bouton relevé->enfoncé
					button->relief = ei_relief_sunken;
					ei_app_invalidate_rect(&widget->screen_location);
				}
				return EI_TRUE;
			} else if (event->type == ei_ev_mouse_buttonup) {
				button->callback(widget, event, button->user_param);
				if (button->relief != ei_relief_raised) {
				        //affichage bouton relevé
					button->relief = ei_relief_raised;
					ei_app_invalidate_rect(&widget->screen_location);
				}
				return EI_TRUE;
			}
		} else if (event->type == ei_ev_mouse_move) {
			if (button->relief != ei_relief_raised) {
			        //affichage bouton relevé pour déplacement hors du bouton
				button->relief = ei_relief_raised;
				ei_app_invalidate_rect(&widget->screen_location);
			}
			return EI_TRUE;
		}
	}
	return EI_FALSE;
}

ei_toplevel_t ei_init_default_toplevel(void) {
	ei_toplevel_t toplevel;
	// toplevel.widget is not initialized
	toplevel.color = ei_default_background_color;
	toplevel.border_width = 4;
	toplevel.title = "Toplevel";
	toplevel.closable = EI_TRUE;
	toplevel.resizable = ei_axis_both;
	toplevel.min_size = ei_size(160, 120);
	toplevel.move_mode.move_mode_bool = EI_FALSE;
	toplevel.resize_mode.resize_mode_bool = EI_FALSE;
	return toplevel;
}

ei_widgetclass_t ei_init_toplevel_class(void) {
	ei_widgetclass_t wclass;
	strcpy(wclass.name, "toplevel");
	wclass.allocfunc = &toplevel_allocfunc;
	wclass.releasefunc = &toplevel_releasefunc;
	wclass.drawfunc = &toplevel_drawfunc;
	wclass.setdefaultsfunc = &toplevel_setdefaultsfunc;
	wclass.geomnotifyfunc = &toplevel_geomnotifyfunc;
	wclass.handlefunc = &toplevel_handlefunc;
	wclass.next = NULL;
	return wclass;
}

ei_widget_t *toplevel_allocfunc(void) {
	ei_widget_t *widget = malloc(sizeof(ei_toplevel_t));
	return widget;
}

void toplevel_releasefunc(ei_widget_t *widget) {
	ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;

	// Free widget fields allocated by library
	ei_placer_forget(widget);
	free(widget->pick_color);
}

void
toplevel_drawfunc(ei_widget_t *widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t *clipper) {
	struct ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
	ei_color_t blanc = {255, 255, 255, 255};
	draw_toplevel(surface, toplevel->title, ei_default_font, blanc, clipper, widget->screen_location,
		      toplevel->color, EI_FALSE, toplevel->border_width);
	draw_toplevel(pick_surface, NULL, ei_default_font, blanc, clipper, widget->screen_location,
		      *widget->pick_color, EI_TRUE, toplevel->border_width);
}

void toplevel_setdefaultsfunc(ei_widget_t *widget) {
	ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
	ei_widgetclass_t *wclass = widget->wclass;
	*toplevel = ei_init_default_toplevel();
	widget->wclass = wclass;
	widget->requested_size = ei_size(320, 240);
	widget->content_rect = malloc(sizeof(ei_rect_t));
}

void toplevel_geomnotifyfunc(ei_widget_t *widget, ei_rect_t rect) {
	*(widget->content_rect) = rect;
}

ei_bool_t mouse_in_window_bar(ei_toplevel_t *toplevel, ei_event_t *event, ei_size_t text_size) {
	int x_bar_min = toplevel->widget.screen_location.top_left.x;
	int x_bar_max = toplevel->widget.screen_location.top_left.x + toplevel->widget.screen_location.size.width;
	int y_bar_min = toplevel->widget.screen_location.top_left.y;
	int y_bar_max = toplevel->widget.screen_location.top_left.y + text_size.height + toplevel->border_width;
	int x_mouse = event->param.mouse.where.x;
	int y_mouse = event->param.mouse.where.y;

	return (x_mouse >= x_bar_min && x_mouse <= x_bar_max && y_mouse >= y_bar_min && y_mouse <= y_bar_max);
}

ei_bool_t mouse_in_window_resize(ei_toplevel_t *toplevel, ei_event_t *event, ei_size_t text_size) {
	int x_resize_min = toplevel->widget.screen_location.top_left.x + toplevel->widget.screen_location.size.height * 0.9;
	int x_resize_max = toplevel->widget.screen_location.top_left.x + toplevel->widget.screen_location.size.width;
	int y_resize_min = toplevel->widget.screen_location.top_left.y + toplevel->widget.screen_location.size.height * 0.9;
	int y_resize_max = toplevel->widget.screen_location.top_left.y + toplevel->widget.screen_location.size.height;
	int x_mouse = event->param.mouse.where.x;
	int y_mouse = event->param.mouse.where.y;

	return (x_mouse >= x_resize_min && x_mouse <= x_resize_max && y_mouse >= y_resize_min && y_mouse <= y_resize_max);
}

ei_bool_t toplevel_handlefunc(ei_widget_t *widget, ei_event_t *event) {
	if (is_located_event(*event)) {
		struct ei_toplevel_t *toplevel = (ei_toplevel_t *) widget;
		ei_size_t size;
		hw_text_compute_size(toplevel->title, ei_default_font, &size.width, &size.height);
		int x_mouse = event->param.mouse.where.x;
		int y_mouse = event->param.mouse.where.y;

		// Souris sur le bandeau de la fenêtre
		if (mouse_in_window_bar(toplevel, event, size)) {
			if (event->type == ei_ev_mouse_buttondown) {
			        //On passe en mode déplacement de la fenêtre
				toplevel->move_mode.move_mode_bool = EI_TRUE;
				toplevel->move_mode.last_location = ei_point(x_mouse, y_mouse);
				return EI_TRUE;
			}
		}

		// Souris qui déplace la fenêtre
		if (event->type == ei_ev_mouse_move && toplevel->move_mode.move_mode_bool) {
			int dx = x_mouse - toplevel->move_mode.last_location.x;
			int dy = y_mouse - toplevel->move_mode.last_location.y;
			int new_x = widget->screen_location.top_left.x + dx;
			int new_y = widget->screen_location.top_left.y + dy;
			ei_app_invalidate_rect(&widget->screen_location);
			ei_place(widget, NULL, &new_x, &new_y, NULL, NULL, NULL, NULL, NULL, NULL);
			ei_app_invalidate_rect(&widget->screen_location);
			toplevel->move_mode.last_location = ei_point(x_mouse, y_mouse);
			return EI_TRUE;
		} else if (event->type == ei_ev_mouse_buttonup && toplevel->move_mode.move_mode_bool) {
		        //On sort du mode déplacement quand le bouton de la souris est relaché
			toplevel->move_mode.move_mode_bool = EI_FALSE;
			toplevel->move_mode.last_location = ei_point(x_mouse, y_mouse);
			return EI_TRUE;
		}

		// Souris sur le bandeau de redimensionnement
		if (mouse_in_window_resize(toplevel, event, size)) {
			if (event->type == ei_ev_mouse_buttondown) {
			        //On passe en mode redimensionnement
				toplevel->resize_mode.resize_mode_bool = EI_TRUE;
				toplevel->resize_mode.last_location = ei_point(x_mouse, y_mouse);
				return EI_TRUE;
			}
		}

		// Souris qui redimensionne la fenêtre
		if (event->type == ei_ev_mouse_move && toplevel->resize_mode.resize_mode_bool) {
                        //Redimensionnement selon les axes voulus
                        int dx = 0; int dy = 0;
		        if (toplevel->resizable == ei_axis_both) {
                                dx = x_mouse - toplevel->resize_mode.last_location.x;
                                dy = y_mouse - toplevel->resize_mode.last_location.y;
		        } else if (toplevel->resizable == ei_axis_x) {
                                dx = x_mouse - toplevel->resize_mode.last_location.x;
		        } else if (toplevel->resizable == ei_axis_y) {
                                dy = y_mouse - toplevel->resize_mode.last_location.y;
                        }

			int new_width = widget->screen_location.size.width + dx;
			int new_height = widget->screen_location.size.height + dy;
			toplevel->resize_mode.last_location = ei_point(x_mouse, y_mouse);
			//Respect de la taille minimale
			if (new_width < toplevel->min_size.width && new_height < toplevel->min_size.height) {
				return EI_FALSE;
			} else {
				if (new_width < toplevel->min_size.width) {
					new_width = widget->screen_location.size.width;
				}
				if (new_height < toplevel->min_size.height) {
					new_height = widget->screen_location.size.height;
				}
			}
			ei_app_invalidate_rect(&widget->screen_location);
			ei_place(widget, NULL, NULL, NULL, &new_width, &new_height, NULL, NULL, NULL, NULL);
			ei_app_invalidate_rect(&widget->screen_location);
			return EI_TRUE;
		} else if (event->type == ei_ev_mouse_buttonup && toplevel->resize_mode.resize_mode_bool) {
		        //On sort du mode redimensionnement
			toplevel->resize_mode.resize_mode_bool = EI_FALSE;
			toplevel->resize_mode.last_location = ei_point(x_mouse, y_mouse);
			return EI_TRUE;
		}
	}
	return EI_FALSE;
}

void draw_toplevel(ei_surface_t surface,
		   const char *text,
		   ei_font_t font,
		   ei_color_t text_color,
		   const ei_rect_t *clipper,
		   ei_rect_t rect,
		   ei_color_t toplevel_color,
		   ei_bool_t pick,
		   int border_width) {
	is_pick_surface = pick;
	if (pick) {
		ei_linked_point_t *pts = rounded_frame(rect, 0, EI_TRUE, EI_TRUE);
		ei_draw_polygon(surface, pts, toplevel_color, clipper);
		free_points(pts);
	} else {
		ei_size_t size;
		hw_text_compute_size(text, font, &(size.width), &(size.height));
		ei_rect_t bot_right_corner; //carré de redimensionnement
		bot_right_corner.size.width = 0.1 * rect.size.height;
		bot_right_corner.size.height = 0.1 * rect.size.height;
		bot_right_corner.top_left.x = rect.top_left.x + rect.size.width - bot_right_corner.size.width;
		bot_right_corner.top_left.y = rect.top_left.y + rect.size.height - bot_right_corner.size.height;

		//dessin de la partie extérieure
		ei_linked_point_t *pts = rounded_frame(rect, 0, EI_TRUE, EI_TRUE);
		ei_color_t frame_color = {toplevel_color.red * 0.5, toplevel_color.green * 0.5,
					  toplevel_color.blue * 0.5, toplevel_color.alpha};
		ei_draw_polygon(surface, pts, frame_color, clipper);
		free_points(pts);

		//position du texte en fonction du rectangle de départ
		ei_point_t where;
		where.x = rect.top_left.x + rect.size.width * 0.05;
		where.y = rect.top_left.y + border_width;

		//rectangle intérieur
		rect.top_left.x += border_width;
		rect.top_left.y += 2 * border_width + size.height;
		rect.size.width -= 2 * border_width;
		rect.size.height = rect.size.height - size.height - 3 * border_width;

		//dessin de la partie intérieure
		pts = rounded_frame(rect, 0, EI_TRUE, EI_TRUE);
		ei_draw_polygon(surface, pts, toplevel_color, clipper);
		free_points(pts);

		//dessin du carré de redimensionnement
		pts = rounded_frame(bot_right_corner, 0, EI_TRUE, EI_TRUE);
		ei_draw_polygon(surface, pts, frame_color, clipper);
		free_points(pts);
		ei_draw_text(surface, &where, text, font, text_color, clipper);
	}

}

void draw_frame(ei_surface_t surface,
		const char *text,
		ei_font_t font,
		ei_color_t text_color,
		const ei_rect_t *clipper,
		ei_rect_t rect,
		ei_color_t frame_color,
		ei_relief_t relief,
		ei_bool_t pick) {
	ei_color_t top_color;
	ei_color_t bot_color;
	is_pick_surface = pick;
	if (pick) {
		ei_linked_point_t *pts;
		pts = rounded_frame(rect, 0, EI_TRUE, EI_TRUE);
		ei_draw_polygon(surface, pts, frame_color, clipper);
		free_points(pts);
		ei_point_t where;
		where.x = rect.top_left.x + rect.size.width * 1.5 / 10;
		where.y = rect.top_left.y + rect.size.height * 3 / 10;
		ei_draw_text(surface, &where, text, font, text_color, clipper);
		return;
	} else {
		if (relief == ei_relief_sunken) {
			top_color.red = frame_color.red * 0.9;
			top_color.green = frame_color.green * 0.9;
			top_color.blue = frame_color.blue * 0.9, top_color.alpha = frame_color.alpha;
                        if (frame_color.red * 1.1 <= 255) {
                                bot_color.red = frame_color.red * 1.1;
                        } else {
                                bot_color.red = 255;
                        }
                        if (frame_color.green * 1.1 <= 255) {
                                bot_color.green = frame_color.green * 1.1;
                        } else {
                                bot_color.red = 255;
                        }
                        if (frame_color.blue * 1.1 <= 255) {
                                bot_color.blue = frame_color.blue * 1.1;
                        } else {
                                bot_color.red = 255;
                        }
			bot_color.alpha = frame_color.alpha;
		} else {
                        if (frame_color.red * 1.1 <= 255) {
                                top_color.red = frame_color.red * 1.1;
                        } else {
                                top_color.red = 255;
                        }
                        if (frame_color.green * 1.1 <= 255) {
                                top_color.green = frame_color.green * 1.1;
                        } else {
                                top_color.red = 255;
                        }
                        if (frame_color.blue * 1.1 <= 255) {
                                top_color.blue = frame_color.blue * 1.1;
                        } else {
                                top_color.red = 255;
                        }
			top_color.alpha = frame_color.alpha;
			bot_color.red = frame_color.red * 0.9;
			bot_color.green = frame_color.green * 0.9;
			bot_color.blue = frame_color.blue * 0.9, bot_color.alpha = frame_color.alpha;
		}
		ei_linked_point_t *pts = rounded_frame(rect, 0, EI_TRUE, EI_TRUE);
		ei_draw_polygon(surface, pts, top_color, clipper);
		free_points(pts);
		pts = rounded_frame(rect, 0, EI_TRUE, EI_TRUE);
		ei_draw_polygon(surface, pts, bot_color, clipper);
		free_points(pts);
		rect.top_left.x += rect.size.width / 20;
		rect.top_left.y += rect.size.height / 20;
		rect.size.width -= rect.size.width * 2 / 20;
		rect.size.height -= rect.size.width * 2 / 20;
		pts = rounded_frame(rect, 0, EI_TRUE, EI_TRUE);
		ei_draw_polygon(surface, pts, frame_color, clipper);
		free_points(pts);
		ei_point_t where;
		where.x = rect.top_left.x + rect.size.width * 1.5 / 10;
		where.y = rect.top_left.y + rect.size.height * 3 / 10;
		ei_draw_text(surface, &where, text, font, text_color, clipper);
	}
}