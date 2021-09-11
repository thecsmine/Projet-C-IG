/**
 *  @file	ei_application_utils.h
 *  @brief	Functions that are useful to \ref ei_application.h
 *
 */

#ifndef EI_APPLICATION_UTILS_H
#define EI_APPLICATION_UTILS_H

#include "ei_event.h"
#include "ei_types.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

/**
 * \brief	Sets the pick surface (for global usage)
 *
 * @param 	surface
 */
void ei_set_pick_surface(ei_surface_t surface);

/**
 * \brief 	Returns the pick surface (for global usage)
 *
 * @return 		The pick surface
 */
ei_surface_t ei_get_pick_surface(void);

/**
 * \brief 	Returns a boolean which is true if and only if "event" is a located event
 *
 * @param 	event
 *
 * @return		boolean
 */
ei_bool_t is_located_event(ei_event_t event);

/**
 * \brief	Returns the intersection rectangle of the two parameters "r1" and "r2"
 *
 * @param 	r1
 * @param 	r2
 * @return 		The intersection of "r1" and "r2"
 */
ei_rect_t rect_intersection(ei_rect_t r1, ei_rect_t r2);

/**
 * \brief	Returns the union rectangle of the two parameters "r1" and "r2"
 *
 * @param 	r1
 * @param 	r2
 * @return 		The union of "r1" and "r2"
 */
ei_rect_t rect_union(ei_rect_t r1, ei_rect_t r2);


/**
 * \brief 	Draws recursively "widget" and every other widgets beside or below in hierarchy.
 * 		Can be used to draw every widget when used with root widget.
 *
 * @param 	widget
 *
 * @param 	root_window	The root window of the application, returned by \ref ei_app_root_surface
 *
 * @param 	clipper		See documentation of draw functions
 */
void draw_widget_recursively(ei_widget_t *widget, ei_surface_t root_window, ei_rect_t *clipper);

/**
 * \brief	Frees the root window
 *
 * @param 	root_window
 */
void free_root_window(ei_surface_t root_window);

/**
 * \brief	Returns the rectangle corresponding to the union of every rectangle in "rectangle_list"
 *
 * @param 	rectangle_list
 *
 * @return 			The rectangle corresponding to the union of every rectangle in "rectangle_list"
 */
ei_rect_t big_union_rect(ei_linked_rect_t *rectangle_list);

/**
 * \brief	Frees "rectangle_list"
 *
 * @param 	rectangle_list
 */
void free_rectangle_list(ei_linked_rect_t *rectangle_list);

#endif //EI_APPLICATION_UTILS_H