/**
 *  @file	ei_draw_utils.h
 *  @brief	Functions that are useful to \ref ei_draw.h
 *
 */

#ifndef EI_DRAW_UTILS_H
#define EI_DRAW_UTILS_H

#include "hw_interface.h"
#include "ei_types.h"


#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

typedef struct ei_side {
	int ymax;	///< Maximum y of the side
	int x_ymin;	///< x corresponding to pixel intersection with scanline of ymin
	int dx;         ///< dx can be negative
	int dy;		///< dy always positive (segment from ymin to ymax)
	int E;          ///< Error in Bresenham algorithm
	struct ei_side *next;
} ei_side;

typedef struct ei_side_table {
	size_t length;
	struct ei_side **array;
} ei_side_table;

/**
 * \brief Boolean allowing draw functions to only use mono-colors (no alpha used)
 */
ei_bool_t is_pick_surface;

/**
 * \brief 	Do the opposite of \ref ei_map_rgba. Converts a 32 bits integer returned by \ref hw_surface_get_buffer
 * 		into the red, green, blue and alpha components.
 *
 * @param 	surface
 * @param 	pixel
 * @return			Red, green, blue and alpha components
 */
ei_color_t pixel_to_rgba(ei_surface_t surface, uint32_t pixel);

/**
 * \brief       Determines if point (x, y) is in clipper
 *
 * @param       x
 * @param       y
 * @param       clipper         If not NULL, the drawing is restricted within this rectangle.
 * @return                      0 iff point not in clipper
 */
ei_bool_t point_in_clipper(int x, int y, const ei_rect_t *clipper);

/**
 * \brief	Draw pixel where pixel_ptr is pointing
 *
 * @param 	surface
 * @param 	pixel_ptr
 * @param 	x
 * @param 	y
 * @param 	color
 * @param	clipper
 * @param	alpha		If false, exact copy of color. If true, weighted copy with alpha.
 */
void draw_pixel(ei_surface_t surface, uint32_t *pixel_ptr, int x, int y, ei_color_t *color, const ei_rect_t *clipper, ei_bool_t alpha);

/**
 * \brief	Add pixels "src_pixel" and "dst_pixel". If alpha is TRUE, weight with pixels' alpha. If alpha is FALSE,
 * 		return the exact copy of "src_pixel".
 *
 * @param	source
 * @param 	src_pixel	32 bits integer returned by \ref hw_surface_get_buffer.
 * 				If NULL, uses "src_color". Both cannot be NULL.
 * @param	src_color	If NULL, uses "src_pixel". Both cannot be NULL.
 * @param 	destination
 * @param 	dst_pixel	32 bits integer returned by \ref hw_surface_get_buffer.
 * @param 	alpha		If false, exact copy of "src_pixel". If true, weighted copy with alpha.
 * @return			Corresponding color
 */
uint32_t add_pixels(ei_surface_t source, uint32_t *src_pixel, ei_color_t *src_color, ei_surface_t destination, uint32_t *dst_pixel, ei_bool_t alpha);

/**
 * \brief       Draw a straight segment.
 *
 * @param       surface         Where to draw the line, from (x1, y1) to (x2, y2).
 *                              The surface must be *locked* by \ref hw_surface_lock.
 * @param       x1
 * @param       x2
 * @param       y1
 * @param       y2
 * @param	color		The color used to draw the line. The alpha channel is managed.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void draw_segment_straight(ei_surface_t surface,
			   int x1, int x2, int y1, int y2,
			   ei_color_t color,
			   const ei_rect_t *clipper);

/**
 * \brief       Draw a segment using Bresenham algorithm.
 * @param       surface         Where to draw the line, from (x1, y1) to (x2, y2).
 *                              The surface must be *locked* by \ref hw_surface_lock.
 *
 * @param       x1
 * @param       y1
 * @param       dx		dx > 0
 * @param       dy		dy > 0
 * @param       sign_x          1 or -1 ; sign of x
 * @param       sign_y          1 or -1 ; sign of y
 * @param       swap            0 or 1 ; determines whether x and y coordinates are swapped
 * @param	color		The color used to draw the line. The alpha channel is managed.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void draw_segment_bresenham(ei_surface_t surface,
			    int x1, int y1, int dx, int dy, int sign_x, int sign_y, int swap,
			    ei_color_t color,
			    const ei_rect_t *clipper);

/**
 * \brief	Construct the side table of polygon defined by "first_point"
 *
 * @param 	surface
 * @param 	first_point
 * @return
 */
ei_side_table construct_side_table(ei_surface_t surface, const ei_linked_point_t *first_point);

/**
 * \brief 	Add "sides" to "*tca"
 *
 * @param 	sides
 * @param 	tca
 */
void move_sides_to_tca(ei_side_table *tc, int y, ei_side **tca);

/**
 * \brief	Delete all sides from tca that are such as "side->ymax == y"
 *
 * @param 	tca
 * @param 	y
 */
void delete_ymax_from_tca(ei_side **tca, int y);

/**
 * \brief	Swap all parameters of sides "s1" and "s2"
 *
 * @param 	s1
 * @param 	s2
 */
void swap_sides(ei_side *s1, ei_side *s2);

/**
 * \brief 	Sort linked list of sides "side" according to its parameter "side->x_ymin"
 *
 * @param 	side
 */
void sort_side_table(ei_side *side);

/**
 * \brief	Draw the scanline corresponding to "tca". Puts "pixel_ptr" to next scanline at the end.
 *
 * @param 	surface
 * @param 	pixel_ptr 	Must be on beginning of the scanline (x=0)
 * @param 	tca
 * @param 	color
 * @param 	clipper
 */
void draw_scanline(ei_surface_t surface, uint32_t **pixel_ptr, ei_side *tca, int y, ei_color_t color,
		   const ei_rect_t *clipper);

/**
 * \brief	Find intersection between scanline "y" and "side"
 *
 * @param 	y
 * @param 	side
 * @return 			The point of the intersection
 */
ei_point_t find_intersection(int y, ei_side *side);

/**
 * \brief 	Update x_ymin (intersection with scanline coordinate) of each side in "tca".
 * 		Uses Bresenham algorithm to determine intersection between segment and scanline "y".
 *
 * @param 	tca
 * @param 	y
 */
void update_scanline(ei_side *tca, int y);

#endif //EI_DRAW_UTILS_H