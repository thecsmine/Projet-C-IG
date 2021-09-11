/**
 *  @file	ei_placer_utils.h
 *  @brief	Functions that are useful to \ref ei_placer.h
 *
 */


#ifndef EI_PLACER_UTILS_H
#define EI_PLACER_UTILS_H

#include "ei_widget.h"

/**
 * \brief	Represents anchor in floats to help calculate (left / right / up / down)-most points.
 * 		For example, with "ei_anc_center", the left-most x coordinate is calculated by
 * 		adding "-0.5 * width" to the x coordinate of the anchor point.
 */
struct anchor_shift {
	float up_direction;
	float down_direction;
	float left_direction;
	float right_direction;
};

struct double_int {
	int left;
	int right;
};

/**
 * \brief 	Initializes the field "placer_params" of "widget" if it is not NULL (allocates its memory).
 *
 * @param 	widget
 */
void init_placer_params(struct ei_widget_t *widget);

/**
 * \brief 	Destroy the field "placer_params" of "widget" if it is not NULL (frees its memory).
 *
 * @param 	widget
 */
void forget_placer_params(ei_widget_t *widget);

/**
 * \brief 	Manages the "anchor" and "anchor_data" fields of "widget->placer_params"
 *
 * @param 	widget
 * @param 	anchor
 */
void manage_anchor(ei_widget_t *widget, ei_anchor_t *anchor);

/**
 * \brief 	Manages the "x", "x_data", "rx" and "rx_data" fields of "widget->placer_params"
 *
 * @param 	widget
 * @param 	x
 * @param 	rel_x
 */
void manage_coord_x(ei_widget_t *widget, int *x, float *rel_x);

/**
 * \brief 	Manages the "y", "y_data", "ry" and "ry_data" fields of "widget->placer_params"
 * @param 	widget
 * @param 	y
 * @param 	rel_y
 */
void manage_coord_y(ei_widget_t *widget, int *y, float *rel_y);

/**
 * \brief Creates struct anchor_shift with parameters "up", "down", "left" and "right"
 *
 * @param up
 * @param down
 * @param left
 * @param right
 *
 * @return		struct anchor_shift
 */
struct anchor_shift init_anchor_shift(float up, float down, float left, float right);

/**
 * 		Initiates struct anchor_shift given the structure of type \ref ei_anchor_t
 * @param 	anchor
 * @return		struct anchor_shift
 */
struct anchor_shift create_anchor_shift(ei_anchor_t anchor);

/**
 * \brief 	Manages the "w", "w_data", "rw" and "rw_data" fields of "widget->placer_params"
 * @param widget
 * @param width
 * @param rel_width
 */
void manage_width(ei_widget_t *widget, int *width, float *rel_width);

/**
 * \brief 	Manages the "h", "h_data", "rh" and "rh_data" fields of "widget->placer_params"
 * @param widget
 * @param height
 * @param rel_height
 */
void manage_height(ei_widget_t *widget, int *height, float *rel_height);

/**
 * \brief 	Determines the field "screen_location" of "widget"
 *
 * @param 	widget
 */
void manage_screen_location(ei_widget_t *widget);

#endif //EI_PLACER_UTILS_H