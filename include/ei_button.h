/**
 *  @file	ei_button.h
 *  @brief	Functions that are useful for drawing buttons
 *
 */


#ifndef PROJETC_IG_EI_BUTTON_H
#define PROJETC_IG_EI_BUTTON_H

#include <stdint.h>
#include "ei_types.h"

void free_points(ei_linked_point_t *ptr);

/**
 * \brief 	Return a linked point that corresponds to the arc.
 *
 * @param 	ei_point_t centre   The centre of the arc.
 * @param 	rayon               The ray of the arc.
 * @param 	debut               The beginning angle of the arc.
 * @param 	fin                 The ending angle of the arc.
 * @return			a linked point
 */
ei_linked_point_t *arc(ei_point_t centre,
		       float rayon,
		       float debut,
		       float fin);

/**
 * \brief 	Return a linked point that corresponds to rounded frame.
 *
 * @param 	rect        The rectangle where the frame is
 * @param 	rayon       The ray of the corners
 * @param 	top_part    The boolean to know if there is the top part.
 * @param 	bot_part    The boolean to know if there is the bot part.
 * @return			a linked point
 */
ei_linked_point_t *rounded_frame(ei_rect_t rect,
				 float rayon,
				 ei_bool_t top_part,
				 ei_bool_t bot_part);

/**
 * \brief 	Draw a button in relief.
 *
 * @param	surface 	Where to draw the text. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	text		The string of the text. Can't be NULL.
 * @param	font		The font used to render the text. If NULL, the \ref ei_default_font
 *				is used.
 * @param	color		The text color. Can't be NULL. The alpha parameter is not used.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 * @param 	rect            The rectangle where to draw the button.
 * @param       button_color    The color of the part inside the button.
 * @param       rayon           The ray of the corners of the button.
 * @param       relief          Relief of the button.
 * @param       pick            Boolean to know if the surface is a pick surface.
 * @return			nothing
 */
void draw_button(ei_surface_t surface,
		 const char *text,
		 ei_font_t font,
		 ei_color_t text_color,
		 const ei_rect_t *clipper,
		 ei_rect_t rect,
		 ei_color_t button_color,
		 float rayon,
		 ei_relief_t relief,
		 ei_bool_t pick);

#endif //PROJETC_IG_EI_BUTTON_H
