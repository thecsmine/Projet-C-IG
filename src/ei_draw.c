#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hw_interface.h"
#include "ei_draw.h"
#include "ei_types.h"

#include "ei_application_utils.h"
#include "ei_draw_utils.h"

/**
* \brief	Converts the red, green, blue and alpha components of a color into a 32 bits integer
* 		than can be written directly in the memory returned by \ref hw_surface_get_buffer.
* 		The surface parameter provides the channel order.
*
* @param	surface		The surface where to store this pixel, provides the channels order.
* @param	color		The color to convert.
*
* @return 			The 32 bit integer corresponding to the color. The alpha component
*				of the color is ignored in the case of surfaces that don't have an
*				alpha channel.
*/
uint32_t ei_map_rgba(ei_surface_t surface, ei_color_t color) {
        int ir, ig, ib, ia;

        /* Obtenir les indices et ordonner dans un tableau */
        hw_surface_get_channel_indices(surface, &ir, &ig, &ib, &ia);
        uint8_t array[4] = {255, 255, 255, 255};
        array[ir] = color.red;
        array[ig] = color.green;
        array[ib] = color.blue;
        if (ia != -1) {
                array[ia] = color.alpha;
        }

        /* Décalage à la bonne position des composantes */
        uint32_t rgba = (array[3] << 24) + (array[2] << 16) + (array[1] << 8) + (array[0]);

        return rgba;
}


/**
 * \brief	Draws a line that can be made of many line segments.
 *
 * @param	surface 	Where to draw the line. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	first_point 	The head of a linked list of the points of the polyline. It can be NULL
 *				(i.e. draws nothing), can have a single point, or more.
 *				If the last point is the same as the first point, then this pixel is
 *				drawn only once.
 * @param	color		The color used to draw the line. The alpha channel is managed.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void ei_draw_polyline(ei_surface_t surface,
                      const ei_linked_point_t *first_point,
                      ei_color_t color,
                      const ei_rect_t *clipper) {
        int x1, x2, y1, y2, dx, dy, sign_x, sign_y;
        int swap;

        if (first_point == NULL) {
                return;
        } else if (first_point->next == NULL) {
                x1 = first_point->point.x;
                y1 = first_point->point.y;

                draw_segment_straight(surface, x1, x1, y1, y1, color, clipper);
                return;
        }

        /* Segment par segment */
        while (first_point->next != NULL) {
                x1 = first_point->point.x;
                y1 = first_point->point.y;
                first_point = first_point->next;
                x2 = first_point->point.x;
                y2 = first_point->point.y;

                dx = x2 - x1;
                dy = y2 - y1;

                /* Conditions à respecter */
                if (dx < 0) {
                        dx = -dx;
                        sign_x = -1;
                } else if (dx == 0) {
                        draw_segment_straight(surface, x1, x2, y1, y2, color, clipper);
                        continue;
                } else {
                        sign_x = 1;
                }
                if (dy < 0) {
                        dy = -dy;
                        sign_y = -1;
                } else if (dy == 0) {
                        draw_segment_straight(surface, x1, x2, y1, y2, color, clipper);
                        continue;
                } else {
                        sign_y = 1;
                }
                if (dx < dy) {
                        swap = 1;
                } else {
                        swap = 0;
                }
                draw_segment_bresenham(surface, x1, y1, dx, dy, sign_x, sign_y, swap, color, clipper);
        }
}

/**
 * \brief	Draws a filled polygon.
 *
 * @param	surface 	Where to draw the polygon. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	first_point 	The head of a linked list of the points of the line. It is either
 *				NULL (i.e. draws nothing), or has more than 2 points. The last point
 *				is implicitly connected to the first point, i.e. polygons are
 *				closed, it is not necessary to repeat the first point.
 * @param	color		The color used to draw the polygon. The alpha channel is managed.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void ei_draw_polygon(ei_surface_t surface,
                     const ei_linked_point_t *first_point,
                     ei_color_t color,
                     const ei_rect_t *clipper) {
        int y = 0;
        int height = hw_surface_get_size(surface).height;
        ei_side_table tc = construct_side_table(surface, first_point);
        ei_side *tca = NULL;
        uint32_t *pixel_ptr = (uint32_t *) hw_surface_get_buffer(surface);

        while (((tc.length != 0) || (tca != NULL)) && y < height) {
                // Déplacer les côtés de TC(y) dans TCA
                move_sides_to_tca(&tc, y, &tca);

                // Supprimer de TCA les côtés tels que ymax = y
                delete_ymax_from_tca(&tca, y);

                // Trier TCA par x_ymin
                sort_side_table(tca);

                // Modifier les pixels de l’image sur la scanline, dans les intervalles intérieurs au polygone
                // pixel_ptr est placée à la prochaine scanline à la fin de draw_scanline
                draw_scanline(surface, &pixel_ptr, tca, y, color, clipper);

                // Incrémenter y
                y++;

                // Mettre à jour les abscisses d’intersections des côtés de TCA avec la nouvelle scanline
                update_scanline(tca, y);
        }
	ei_side *ptr;
	while (tca != NULL && tca->next != NULL) {
		ptr = tca->next;
		free(tca);
		tca = ptr;
	}
	free(tca);
        free(tc.array);
}

/**
 * \brief	Draws text by calling \ref hw_text_create_surface.
 *
 * @param	surface 	Where to draw the text. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	where		Coordinates, in the surface, where to anchor the top-left corner of
 *				the rendered text.
 * @param	text		The string of the text. Can't be NULL.
 * @param	font		The font used to render the text. If NULL, the \ref ei_default_font
 *				is used.
 * @param	color		The text color. Can't be NULL. The alpha parameter is not used.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void ei_draw_text(ei_surface_t surface,
                  const ei_point_t *where,
                  const char *text,
                  ei_font_t font,
                  ei_color_t color,
                  const ei_rect_t *clipper) {
	if (text == NULL || strcmp(text, "") == 0) {
		return;
	}
        ei_rect_t dst_rect;
        ei_rect_t src_rect;
        ei_size_t size;
        ei_surface_t text_surface;
        ei_bool_t alpha = EI_TRUE;

        // Compute size and dst_rect
        hw_text_compute_size(text, font, &(size.width), &(size.height));


        // Create src_rect if clipping needed by finding the intersection between the two rectangles
        if (clipper != NULL) {
        	ei_rect_t r2 = {*where, size};
		ei_rect_t r0 = rect_intersection(*clipper, r2);
		size = r0.size;
                if (size.width < 0 || size.height < 0) { // Empty intersection
                        return;
                }
                if (where->x >= clipper->top_left.x) {
                        dst_rect.top_left.x = where->x;
                        src_rect.top_left.x = 0;
                } else {
                        dst_rect.top_left.x = clipper->top_left.x;
                        src_rect.top_left.x = clipper->top_left.x - where->x;
                }
                if (where->y > clipper->top_left.y) {
                        dst_rect.top_left.y = where->y;
                        src_rect.top_left.y = 0;
                } else {
                        dst_rect.top_left.y = clipper->top_left.y;
                        src_rect.top_left.y = clipper->top_left.y - where->y;
                }
                dst_rect.size = size;
                src_rect.size = size;
        } else {
                dst_rect.top_left = *where;
                dst_rect.size = size;
        }

        // Create surface, then lock it
        text_surface = hw_text_create_surface(text, font, color);
        hw_surface_lock(text_surface);

        // Copy surface
        if (clipper == NULL) {
                ei_copy_surface(surface, &dst_rect, text_surface, NULL, alpha);
        } else {
                ei_copy_surface(surface, &dst_rect, text_surface, &src_rect, alpha);
        }

        // Free surface
        hw_surface_unlock(text_surface);
        hw_surface_free(text_surface);
}

/**
 * \brief	Fills the surface with the specified color.
 *
 * @param	surface		The surface to be filled. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	color		The color used to fill the surface. If NULL, it means that the
 *				caller want it painted black (opaque).
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void ei_fill(ei_surface_t surface,
             const ei_color_t *color,
             const ei_rect_t *clipper) {
        ei_size_t size = hw_surface_get_size(surface);
        uint32_t *pixel_ptr;
        ei_bool_t alpha = EI_TRUE;
        int x, y;

        pixel_ptr = (uint32_t *) hw_surface_get_buffer(surface);
        for (y = 0; y < size.height; y++) {
                for (x = 0; x < size.width; x++){
                        draw_pixel(surface, pixel_ptr, x, y, color, clipper, alpha);
                        pixel_ptr++;
                }
        }
}


/**
 * \brief	Copies pixels from a source surface to a destination surface.
 *		The source and destination areas of the copy (either the entire surfaces, or
 *		subparts) must have the same size before considering clipping.
 *		Both surfaces must be *locked* by \ref hw_surface_lock.
 *
 * @param	destination	The surface on which to copy pixels.
 * @param	dst_rect	If NULL, the entire destination surface is used. If not NULL,
 *				defines the rectangle on the destination surface where to copy
 *				the pixels.
 * @param	source		The surface from which to copy pixels.
 * @param	src_rect	If NULL, the entire source surface is used. If not NULL, defines the
 *				rectangle on the source surface from which to copy the pixels.
 * @param	alpha		If true, the final pixels are a combination of source and
 *				destination pixels weighted by the source alpha channel and
 *				the transparency of the final pixels is set to opaque.
 *				If false, the final pixels are an exact copy of the source pixels,
 				including the alpha channel.
 *
 * @return			Returns 0 on success, 1 on failure (different sizes between source and destination).
 */
int ei_copy_surface(ei_surface_t destination,
                    const ei_rect_t *dst_rect,
                    ei_surface_t source,
                    const ei_rect_t *src_rect,
                    ei_bool_t alpha) {
        int x, y, dst_x0, dst_y0, src_x0, src_y0, dst_newline = 0, src_newline = 0;
        int dst_width, dst_height, src_width, src_height;
        ei_size_t dst_size = hw_surface_get_size(destination);
        ei_size_t src_size = hw_surface_get_size(source);
        uint32_t *dst_pixel = (uint32_t *) hw_surface_get_buffer(destination);
        uint32_t *src_pixel = (uint32_t *) hw_surface_get_buffer(source);

        // Définition des tailles
        if (dst_rect == NULL) {
                dst_width = dst_size.width;
                dst_height = dst_size.height;
        } else {
                dst_width = dst_rect->size.width;
                dst_height = dst_rect->size.height;
                // Positionnement du pixel sur dst_rect->top_left
                dst_x0 = dst_rect->top_left.x;
                dst_y0 = dst_rect->top_left.y;
                dst_pixel += dst_x0 + (dst_size.width * dst_y0);
                dst_newline = dst_size.width - dst_rect->size.width; // incrément pour passer à la ligne suivante
        }
        if (src_rect == NULL) {
                src_width = src_size.width;
                src_height = src_size.height;
        } else {
                src_width = src_rect->size.width;
                src_height = src_rect->size.height;
                // Positionnement du pixel sur src_rect->top_left
                src_x0 = src_rect->top_left.x;
                src_y0 = src_rect->top_left.y;
                src_pixel += src_x0 + (src_size.width * src_y0);
                src_newline = src_size.width - src_rect->size.width; // incrément pour passer à la ligne suivante
        }

        // Vérification des tailles
        if (!(dst_width == src_width && dst_height == src_height)) {
                return 1;
        }

        // Copie de la surface
        for (y = 0; y < src_height; y++) {
                for (x = 0; x < src_width; x++) {
                        *dst_pixel = add_pixels(source, src_pixel, NULL, destination, dst_pixel, alpha);
                        dst_pixel++;
                        src_pixel++;
                }
                dst_pixel += dst_newline;
                src_pixel += src_newline;
        }
        return 0;
}
