/**
 *  @file	ei_widget_utils.h
 *  @brief	Functions that are useful to \ref ei_widget.h
 *
 */


#ifndef EI_WIDGET_UTILS_H
#define EI_WIDGET_UTILS_H

#include "ei_types.h"
#include "ei_widget.h"
#include "ei_widgetclass.h"

typedef struct ei_frame_t {
	ei_widget_t widget; 		///< Doit être de type "ei_widget_t" pour polymorphisme
	ei_color_t color;
	int border_width;
	ei_relief_t relief;
	char *text;
	ei_font_t text_font;
	ei_color_t text_color;
	ei_anchor_t text_anchor;
	ei_surface_t *img;
	ei_rect_t *img_rect;
	ei_anchor_t img_anchor;
	ei_bool_t requested_bool;
} ei_frame_t;

typedef struct ei_button_t {
	ei_widget_t widget; 		///< Doit être de type "ei_widget_t" pour polymorphisme
	ei_color_t color;
	int border_width;
	int corner_radius;
	ei_relief_t relief;
	char *text;
	ei_font_t text_font;
	ei_color_t text_color;
	ei_anchor_t text_anchor;
	ei_surface_t *img;
	ei_rect_t *img_rect;
	ei_anchor_t img_anchor;
	ei_callback_t callback;
	void *user_param;
	ei_bool_t requested_bool;
} ei_button_t;

typedef struct ei_move_mode {
	ei_bool_t move_mode_bool;
	ei_point_t last_location;
} ei_move_mode;

typedef struct ei_resize_mode {
	ei_bool_t resize_mode_bool;
	ei_point_t last_location;
} ei_resize_mode;

typedef struct ei_toplevel_t {
	ei_widget_t widget; 		///< Doit être de type "ei_widget_t" pour polymorphisme
	ei_color_t color;
	int border_width;
	char *title;
	ei_bool_t closable;
	ei_axis_set_t resizable;
	ei_size_t min_size;
	struct ei_move_mode move_mode;
	struct ei_resize_mode resize_mode;
} ei_toplevel_t;

/**
 * \brief	Empty callback function of type \ref ei_callback_t
 *
 * @param 	widget
 * @param 	event
 * @param 	user_param
 */
void empty_callback(ei_widget_t *widget, struct ei_event_t *event, void *user_param);

/**
 * \brief 	Similar to \ref ei_widget_destroy, but used only on widgets that do not need
 * 		their "next_sibling" / "children_head" / "children_tail" fields to be rewritten.
 *
 * @param 	widget
 */
void ei_widget_destroy_child(ei_widget_t *widget);

/**
 * \brief 	Function used to attribuate an unique "pick_id" to "widget".
 *
 * @return		Unique pick id (used for pick surface)
 */
uint32_t ei_get_widget_id(void);

/**
 * \brief 	Finds the widget such as "widget->pick_id == id". Returns NULL is not found
 *
 * @param 	id
 *
 * @return		Widget or NULL corresponding to the id
 */
ei_widget_t *ei_find_widget_by_id(uint32_t id);

/**
 * \brief 	Recursive function used by \ref ei_find_widget_by_id for the search
 *
 * @param 	widget
 * @param 	id
 * @return 		Widget or NULL corresponding to the id
 */
ei_widget_t *search_widget(ei_widget_t *widget, uint32_t id);

/**
 * \brief 	Returns the natural size of a widget (see parameters)
 *
 * @param 	border_width
 * @param 	text
 * @param 	text_font
 * @param 	img_rect
 * @return			natural size
 */
ei_size_t ei_widget_natural_size(int border_width, char *text, ei_font_t text_font, ei_rect_t *img_rect);

/**
 * \brief	Returns a frame with default fields
 *
 * @return		Frame with default fields
 */
ei_frame_t ei_init_default_frame(void);

/**
 * \brief	Returns the frame widgetclass
 *
 * @return		Frame widgetclass
 */
ei_widgetclass_t ei_init_frame_class(void);

/**
 * \brief	Frame alloc function (see \ref ei_widgetclass_allocfunc_t)
 *
 * @return 		Allocated frame
 */
ei_widget_t *frame_allocfunc(void);

/**
 * \brief 	Frame release function (see \ref ei_widgetclass_releasefunc_t)
 *
 * @param 	widget
 */
void frame_releasefunc(ei_widget_t *widget);

/**
 * \brief 	Frame draw function (see \ref ei_widgetclass_drawfunc_t)
 *
 * @param 	widget
 * @param	surface
 * @param	pick_surface
 * @param	clipper
 */
void frame_drawfunc(ei_widget_t *widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t *clipper);

/**
 * \brief 	Frame setdefaults function (see \ref ei_widgetclass_setdefaultsfunc_t)
 *
 * @param 	widget
 */
void frame_setdefaultsfunc(ei_widget_t *widget);

/**
 * \brief 	Frame geomnotify function (see \ref ei_widgetclass_geomnotifyfunc_t)
 *
 * @param 	widget
 * @param	rect
 */
void frame_geomnotifyfunc(ei_widget_t *widget, ei_rect_t rect);

/**
 * \brief 	Frame handle function (see \ref ei_widgetclass_handlefunc_t)
 *
 * @param 	widget
 * @param	event
 *
 * @return		handled boolean
 */
ei_bool_t frame_handlefunc(ei_widget_t *widget, struct ei_event_t *event);

/**
 * \brief	Returns a button with default fields
 *
 * @return		Button with default fields
 */
ei_button_t ei_init_default_button(void);

/**
 * \brief	Returns the button widgetclass
 *
 * @return		Button widgetclass
 */
ei_widgetclass_t ei_init_button_class(void);

/**
 * \brief	Button alloc function (see \ref ei_widgetclass_allocfunc_t)
 *
 * @return 		Allocated button
 */
ei_widget_t *button_allocfunc(void);

/**
 * \brief	Button release function (see \ref ei_widgetclass_releasefunc_t)
 *
 * @return 		Allocated button
 */
void button_releasefunc(ei_widget_t *widget);

/**
 *
 * \brief 	Button draw function (see \ref ei_widgetclass_drawfunc_t)
 * @param 	widget
 * @param 	surface
 * @param 	pick_surface
 * @param 	clipper
 */
void button_drawfunc(ei_widget_t *widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t *clipper);

/**
 * \brief 	Button setdefaults function (see \ref ei_widgetclass_setdefaultsfunc_t)
 *
 * @param 	widget
 */
void button_setdefaultsfunc(ei_widget_t *widget);

/**
 * \brief 	Button geomnotify function (see \ref ei_widgetclass_geomnotifyfunc_t)
 *
 * @param 	widget
 * @param 	rect
 */
void button_geomnotifyfunc(ei_widget_t *widget, ei_rect_t rect);

/**
 * \brief 	Button handle function (see \ref ei_widgetclass_handlefunc_t)
 *
 * @param 	widget
 * @param 	event
 *
 * @return 		handled boolean
 */
ei_bool_t button_handlefunc(ei_widget_t *widget, struct ei_event_t *event);

/**
 * \brief	Returns a toplevel with default fields
 *
 * @return		Toplevel with default fields
 */
ei_toplevel_t ei_init_default_toplevel(void);

/**
 * \brief	Returns the toplevel widgetclass
 *
 * @return		Toplevel widgetclass
 */
ei_widgetclass_t ei_init_toplevel_class(void);

/**
 * \brief	Toplevel alloc function (see \ref ei_widgetclass_allocfunc_t)
 *
 * @return 		Allocated toplevel
 */
ei_widget_t *toplevel_allocfunc(void);

/**
 * \brief 	Toplevel release function (see \ref ei_widgetclass_releasefunc_t)
 *
 * @param 	widget
 */
void toplevel_releasefunc(ei_widget_t *widget);

/**
 * \brief	Toplevel draw function (see \ref ei_widgetclass_drawfunc_t)
 *
 * @param 	widget
 * @param 	surface
 * @param 	pick_surface
 * @param 	clipper
 */
void toplevel_drawfunc(ei_widget_t *widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t *clipper);

/**
 * \brief	Toplevel setdefaults function (see \ref ei_widgetclass_setdefaultsfunc_t)
 *
 * @param 	widget
 */
void toplevel_setdefaultsfunc(ei_widget_t *widget);

/**
 * \brief	Toplevel geomnotify function (see \ref ei_widgetclass_geomnotifyfunc_t)
 *
 * @param 	widget
 * @param 	rect
 */
void toplevel_geomnotifyfunc(ei_widget_t *widget, ei_rect_t rect);

/**
 * \brief	Toplevel alloc function (see \ref ei_widgetclass_allocfunc_t)
 *
 * @param 	widget
 * @param 	event
 *
 * @return 		handled boolean
 */
ei_bool_t toplevel_handlefunc(ei_widget_t *widget, struct ei_event_t *event);

/**
 * \brief	Draws a toplevel
 *
 * @param 	surface
 * @param 	text
 * @param 	font
 * @param 	text_color
 * @param 	clipper
 * @param 	rect
 * @param 	toplevel_color
 * @param 	pick
 * @param 	border_width
 */
void draw_toplevel(ei_surface_t surface,
		   const char *text,
		   ei_font_t font,
		   ei_color_t text_color,
		   const ei_rect_t *clipper,
		   ei_rect_t rect,
		   ei_color_t toplevel_color,
		   ei_bool_t pick,
		   int border_width);

/**
 * \brief	Draws a frame
 *
 * @param 	surface
 * @param 	text
 * @param 	font
 * @param 	text_color
 * @param 	clipper
 * @param 	rect
 * @param 	frame_color
 * @param 	relief
 * @param 	pick
 */
void draw_frame(ei_surface_t surface,
		const char *text,
		ei_font_t font,
		ei_color_t text_color,
		const ei_rect_t *clipper,
		ei_rect_t rect,
		ei_color_t frame_color,
		ei_relief_t relief,
		ei_bool_t pick);

#endif //EI_WIDGET_UTILS_H