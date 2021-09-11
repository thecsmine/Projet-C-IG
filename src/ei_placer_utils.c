#include <stdlib.h>

#include "ei_types.h"
#include "ei_utils.h"
#include "ei_widget.h"

#include "ei_application_utils.h"
#include "ei_placer_utils.h"

void init_placer_params(struct ei_widget_t *widget) {
	if (widget->placer_params != NULL) {
		return;
	}
	// If not managed by placer, create placer_params
	ei_placer_params_t *param = malloc(sizeof(ei_placer_params_t));
	param->anchor = malloc(sizeof(ei_anchor_t));
	param->anchor = NULL;
	param->x = malloc(sizeof(int));
	param->x = NULL;
	param->x_data = 0;
	param->y = malloc(sizeof(int));
	param->y = NULL;
	param->y_data = 0;
	param->w = malloc(sizeof(int));
	param->w = NULL;
	param->w_data = 0;
	param->h = malloc(sizeof(int));
	param->h = NULL;
	param->h_data = 0;
	param->rx = malloc(sizeof(float));
	param->rx = NULL;
	param->rx_data = 0;
	param->ry = malloc(sizeof(float));
	param->ry = NULL;
	param->ry_data = 0;
	param->rw = malloc(sizeof(float));
	param->rw = NULL;
	param->rw_data = 0;
	param->rh = malloc(sizeof(float));
	param->rh = NULL;
	param->rh_data = 0;

	widget->placer_params = param;
}

void forget_placer_params(ei_widget_t *widget) {
	ei_placer_params_t *param = widget->placer_params;
	if (param == NULL) {
		return;
	}
	widget->placer_params = NULL;
}

void manage_anchor(ei_widget_t *widget, ei_anchor_t *anchor) {
	if (anchor != NULL) {
		widget->placer_params->anchor_data = *anchor;
		widget->placer_params->anchor = &(widget->placer_params->anchor_data);
	} else if (widget->placer_params->anchor == NULL) { // no default value
		widget->placer_params->anchor_data = ei_anc_northwest;
		widget->placer_params->anchor = &(widget->placer_params->anchor_data);
	}
}

void manage_coord_x(ei_widget_t *widget, int *x, float *rel_x) {
	// Gestion x
	// 0. rel_x - padding with x or no padding
	// 1. x
	// 2. default value (placer_params)
	// 3. x = 0, rel_x = 0.0

	if (rel_x != NULL && (0 <= *rel_x && *rel_x <= 1)) {
		if (x != NULL) {
			widget->placer_params->rx_data = *rel_x;
			widget->placer_params->rx = &(widget->placer_params->rx_data);
			widget->placer_params->x_data = *x;
			widget->placer_params->x = &(widget->placer_params->x_data);
		} else {
			widget->placer_params->rx_data = *rel_x;
			widget->placer_params->rx = &(widget->placer_params->rx_data);
			widget->placer_params->x_data = 0;
			widget->placer_params->x = NULL;
		}
	} else if (x != NULL) {
		widget->placer_params->rx_data = 0;
		widget->placer_params->rx = NULL;
		widget->placer_params->x_data = *x;
		widget->placer_params->x = &(widget->placer_params->x_data);
	} else if (widget->placer_params->x == NULL && widget->placer_params->rx == NULL) { // no default value
		widget->placer_params->rx_data = 0;
		widget->placer_params->rx = NULL;
		widget->placer_params->x_data = 0;
		widget->placer_params->x = &(widget->placer_params->x_data);
	}
}

void manage_coord_y(ei_widget_t *widget, int *y, float *rel_y) {
	// Gestion y
	// 0. rel_y - padding with y or no padding
	// 1. y
	// 2. default value (placer_params)
	// 3. y = 0, rel_y = 0.0

	if (rel_y != NULL && (0 <= *rel_y && *rel_y <= 1)) {
		if (y != NULL) {
			widget->placer_params->ry_data = *rel_y;
			widget->placer_params->ry = &(widget->placer_params->ry_data);
			widget->placer_params->y_data = *y;
			widget->placer_params->y = &(widget->placer_params->y_data);
		} else {
			widget->placer_params->ry_data = *rel_y;
			widget->placer_params->ry = &(widget->placer_params->ry_data);
			widget->placer_params->y_data = 0;
			widget->placer_params->y = NULL;
		}
	} else if (y != NULL) {
		widget->placer_params->ry_data = 0;
		widget->placer_params->ry = NULL;
		widget->placer_params->y_data = *y;
		widget->placer_params->y = &(widget->placer_params->y_data);
	} else if (widget->placer_params->y == NULL && widget->placer_params->ry == NULL) { // no default value
		widget->placer_params->ry_data = 0;
		widget->placer_params->ry = NULL;
		widget->placer_params->y_data = 0;
		widget->placer_params->y = &(widget->placer_params->y_data);
	}
}

struct anchor_shift init_anchor_shift(float up, float down, float left, float right) {
	struct anchor_shift as = {up, down, left, right};
	return as;
}

struct anchor_shift create_anchor_shift(ei_anchor_t anchor) {
	/*
	ei_anc_center,			///< Anchor in the center.
	ei_anc_north,			///< Anchor on the top side, centered horizontally.
	ei_anc_northeast,		///< Anchor on the top-right corner.
	ei_anc_east,			///< Anchor on the right side, centered vertically.
	ei_anc_southeast,		///< Anchor on the bottom-right corner.
	ei_anc_south,			///< Anchor on the bottom side, centered horizontally.
	ei_anc_southwest,		///< Anchor on the bottom-left corner.
	ei_anc_west,			///< Anchor on the left side, centered vertically.
	ei_anc_northwest
	 */
	struct anchor_shift as;
	switch (anchor) {
		case (ei_anc_center):
			as = init_anchor_shift(-0.5, 0.5, -0.5, 0.5);
			break;
		case (ei_anc_north):
			as = init_anchor_shift(0, 1, -0.5, 0.5);
			break;
		case (ei_anc_northeast):
			as = init_anchor_shift(0, 1, -1, 0);
			break;
		case (ei_anc_east):
			as = init_anchor_shift(-0.5, 0.5, -1, 0);
			break;
		case (ei_anc_southeast):
			as = init_anchor_shift(-1, 0, -1, 0);
			break;
		case (ei_anc_south):
			as = init_anchor_shift(-1, 0, -0.5, 0.5);
			break;
		case (ei_anc_southwest):
			as = init_anchor_shift(-1, 0, 0, 1);
			break;
		case (ei_anc_west):
			as = init_anchor_shift(-0.5, 0.5, 0, 1);
			break;
		case (ei_anc_northwest):
			as = init_anchor_shift(0, 1, 0, 1);
			break;
		default:
			break;
	}
	return as;
}

void manage_width(ei_widget_t *widget, int *width, float *rel_width) {
	// Gestion width
	// 0. Paramètres
	// 1. valeur par default (placer_params)
	// 2. requested_size
	// 3. = 0
	int w = 0, rw = 0;

	if (rel_width != NULL) {
		// rel_width
		rw = *rel_width;
	} else if (width != NULL) {
		// width
		w = *width;
	} else if (widget->placer_params->rw != NULL) {
		// default
		rw = widget->placer_params->rw_data;
	} else if (widget->placer_params->w != NULL) {
		w = widget->placer_params->w_data;
	} else if (widget->requested_size.width != 0) {
		// requested width
		w = widget->requested_size.width;
	} // else zero

	if (rw == 0) {
		widget->placer_params->rw_data = 0;
		widget->placer_params->rw = NULL;
		widget->placer_params->w_data = w;
		widget->placer_params->w = &(widget->placer_params->w_data);
	} else {
		widget->placer_params->w_data = 0;
		widget->placer_params->w = NULL;
		widget->placer_params->rw_data = rw;
		widget->placer_params->rw = &(widget->placer_params->rw_data);
	}
}

void manage_height(ei_widget_t *widget, int *height, float *rel_height) {
	// Gestion height
	// 0. Paramètres
	// 1. valeur default (placer_params)
	// 2. requested_size
	// 3. = 0
	int h = 0, rh = 0;

	if (rel_height != NULL) {
		// rel_height
		rh = *rel_height;
	} else if (height != NULL) {
		// height
		h = *height;
	} else if (widget->placer_params->rh != NULL) {
		// default
		rh = widget->placer_params->rh_data;
	} else if (widget->placer_params->h != NULL) {
		h = widget->placer_params->h_data;
	} else if (widget->requested_size.height != 0) {
		// requested height
		h = widget->requested_size.height;
	} // else zero

	if (rh == 0) {
		widget->placer_params->rh_data = 0;
		widget->placer_params->rh = NULL;
		widget->placer_params->h_data = h;
		widget->placer_params->h = &(widget->placer_params->h_data);
	} else {
		widget->placer_params->h_data = 0;
		widget->placer_params->h = NULL;
		widget->placer_params->rh_data = rh;
		widget->placer_params->rh = &(widget->placer_params->rh_data);
	}
}

struct double_int
get_direction_most(int parent_dimension, int parent_c, float *rel_dimension, int *dimension, float *rel_c, int *c,
		   float left, float right) {
	int pos_c;
	struct double_int lr;
	if (c != NULL) {
		pos_c = *c;
	} else {
		pos_c = parent_c + (*rel_c) * parent_dimension;
	}

	if (dimension != NULL) {
		lr.left = pos_c + (left * (*dimension));
		lr.right = pos_c + (right * (*dimension));
	} else if (rel_dimension != NULL) {
		lr.left = pos_c + (left * (*rel_dimension) * parent_dimension);
		lr.right = pos_c + (right * (*rel_dimension) * parent_dimension);
	}
	return lr;
}

void manage_screen_location(ei_widget_t *widget) {
	ei_rect_t screen_location;
	ei_rect_t parent_rect;
	struct anchor_shift as = create_anchor_shift(widget->placer_params->anchor_data);
	if (widget->parent != NULL) {
		parent_rect = *(widget->parent->content_rect);
	} else {
		parent_rect = ei_rect_zero();
	}
	struct double_int x_coord = get_direction_most(parent_rect.size.width,
						       parent_rect.top_left.x,
						       widget->placer_params->rw, widget->placer_params->w,
						       widget->placer_params->rx, widget->placer_params->x,
						       as.left_direction, as.right_direction);
	struct double_int y_coord = get_direction_most(parent_rect.size.height,
						       parent_rect.top_left.y,
						       widget->placer_params->rh, widget->placer_params->h,
						       widget->placer_params->ry, widget->placer_params->y,
						       as.up_direction, as.down_direction);
	screen_location.top_left.x = x_coord.left;
	screen_location.size.width = x_coord.right - x_coord.left;

	screen_location.top_left.y = y_coord.left;
	screen_location.size.height = y_coord.right - y_coord.left;

	if (widget->parent != NULL) {
		screen_location = rect_intersection(screen_location, parent_rect);
	}
	widget->screen_location = screen_location;
}
