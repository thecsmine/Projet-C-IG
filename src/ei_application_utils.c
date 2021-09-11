#include <stdlib.h>

#include "ei_event.h"
#include "ei_types.h"
#include "ei_widget.h"

#include "ei_application_utils.h"

/** Global variables **/
/**                  **/
ei_surface_t PICK_SURFACE;
/**                  **/
/** ---------------- **/

void ei_set_pick_surface(ei_surface_t surface) {
	PICK_SURFACE = surface;
}

ei_surface_t ei_get_pick_surface(void) {
	return PICK_SURFACE;
}

ei_bool_t is_located_event(ei_event_t event) {
	return (ei_bool_t) (event.type == ei_ev_mouse_buttondown || event.type == ei_ev_mouse_buttonup ||
			    event.type == ei_ev_mouse_move);
}

ei_rect_t rect_intersection(ei_rect_t r1, ei_rect_t r2) {
	ei_rect_t r0;
	r0.top_left.x = max(r1.top_left.x, r2.top_left.x);
	r0.top_left.y = max(r1.top_left.y, r2.top_left.y);
	r0.size.width = min(r1.top_left.x + r1.size.width, r2.top_left.x + r2.size.width) - r0.top_left.x;
	r0.size.height = min(r1.top_left.y + r1.size.height, r2.top_left.y + r2.size.height) - r0.top_left.y;
	return r0;
}

ei_rect_t rect_union(ei_rect_t r1, ei_rect_t r2) {
	ei_rect_t r0;
	r0.top_left.x = min(r1.top_left.x, r2.top_left.x);
	r0.top_left.y = min(r1.top_left.y, r2.top_left.y);
	r0.size.width = max(r1.top_left.x + r1.size.width, r2.top_left.x + r2.size.width) - r0.top_left.x;
	r0.size.height = max(r1.top_left.y + r1.size.height, r2.top_left.y + r2.size.height) - r0.top_left.y;
	return r0;
}

void draw_widget_recursively(ei_widget_t *widget, ei_surface_t root_window, ei_rect_t *clipper) {
	// Traitement pour un widget
	ei_rect_t *current_clipper = malloc(sizeof(ei_rect_t));
	if (clipper == NULL) {
		*current_clipper = widget->screen_location;
	} else {
		*current_clipper = rect_intersection(*clipper, widget->screen_location);
	}
	if (current_clipper->size.width != 0 && current_clipper->size.height != 0) {
		widget->wclass->drawfunc(widget, root_window, PICK_SURFACE, current_clipper);
	}
	free(current_clipper);

	// Prochain widget à traiter
	if (widget->next_sibling != NULL) {
		draw_widget_recursively(widget->next_sibling, root_window, clipper);
	} else if (widget->children_head != NULL) {
		draw_widget_recursively(widget->children_head, root_window, clipper);
	}
}

void free_root_window(ei_surface_t root_window) {
	// Free root window
	hw_surface_unlock(root_window);
	hw_surface_free(root_window);

	// Free pick surface
	hw_surface_unlock(PICK_SURFACE);
	hw_surface_free(PICK_SURFACE);
}

ei_rect_t big_union_rect(ei_linked_rect_t *rectangle_list) {
	ei_rect_t big_rect = rectangle_list->rect;
	rectangle_list = rectangle_list->next;
	while (rectangle_list != NULL) {
		big_rect = rect_union(big_rect, rectangle_list->rect);
		rectangle_list = rectangle_list->next;
	}
	return big_rect;
}

void free_rectangle_list(ei_linked_rect_t *rectangle_list) {
	if (rectangle_list == NULL) {
		return;
	}
	ei_linked_rect_t *ptr;
	while (rectangle_list->next != NULL) {
		ptr = rectangle_list->next;
		free(rectangle_list);
		rectangle_list = ptr;
	}
	free(rectangle_list);
}