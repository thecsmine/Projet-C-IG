#include <stdint.h>
#include <stdlib.h>

#include "hw_interface.h"
#include "ei_draw.h"
#include "ei_types.h"

#include "ei_draw_utils.h"

/** Global variables **/
/**                  **/
ei_bool_t is_pick_surface = EI_FALSE;
/**                  **/
/** ---------------- **/

ei_color_t pixel_to_rgba(ei_surface_t surface, uint32_t pixel) {
	int ir, ig, ib, ia;
	ei_color_t color;

	/* Obtenir les indices et ranger dans un tableau */
	hw_surface_get_channel_indices(surface, &ir, &ig, &ib, &ia);
	int array[4] = {0, 8, 16, 24};
	color.red = (unsigned char) (pixel >> array[ir]) & 0x000000FF;
	color.green = (unsigned char) (pixel >> array[ig]) & 0x000000FF;
	color.blue = (unsigned char) (pixel >> array[ib]) & 0x000000FF;
	if (ia == -1) {
		color.alpha = 0xff;
	} else {
		color.alpha = (unsigned char) (pixel >> array[ia]) & 0x000000FF;
	}
	return color;
}


ei_bool_t point_in_clipper(int x, int y, const ei_rect_t *clipper) {
	if (clipper == NULL) {
		return EI_TRUE;
	} else {
		int cx = clipper->top_left.x;
		int cy = clipper->top_left.y;
		return (ei_bool_t)(cx <= x && x <= (cx + clipper->size.width) && cy <= y && y <= (cy + clipper->size.height));
	}
}

void draw_pixel(ei_surface_t surface, uint32_t *pixel_ptr, int x, int y, ei_color_t *color, const ei_rect_t *clipper, ei_bool_t alpha) {
	if (point_in_clipper(x, y, clipper)) {
		if (color == NULL) {
			*pixel_ptr = (uint32_t) 0x00000000;
		} else {
			*pixel_ptr = add_pixels(surface, NULL, color, surface, pixel_ptr, alpha);
		}
	}
}

uint32_t add_pixels(ei_surface_t source, uint32_t *src_pixel, ei_color_t *src_color, ei_surface_t destination, uint32_t *dst_pixel, ei_bool_t alpha) {
	uint32_t result;
	if (!is_pick_surface && alpha) { // Use additive transparency
		ei_color_t src;
		ei_color_t dst = pixel_to_rgba(destination, *dst_pixel);
		if (src_color == NULL) {
			src = pixel_to_rgba(source, *src_pixel);
		} else {
			src = *src_color;
		}
		dst.red = (src.alpha*src.red + (255 - src.alpha)*dst.red)/255;
		dst.green = (src.alpha*src.green + (255 - src.alpha)*dst.green)/255;
		dst.blue = (src.alpha*src.blue + (255 - src.alpha)*dst.blue)/255;

		return ei_map_rgba(destination, dst);

	} else { // Copy source to result
		if (src_pixel == NULL) {
			result = ei_map_rgba(source, *src_color);
		} else {
			result = *src_pixel;
		}
		return result;
	}
}

void draw_segment_straight(ei_surface_t surface,
			   int x1, int x2, int y1, int y2,
			   ei_color_t color,
			   const ei_rect_t *clipper) {
	int width = hw_surface_get_size(surface).width;
	int i, sign = 1, incr;
	uint32_t *pixel_ptr = (uint32_t *) hw_surface_get_buffer(surface);
	ei_bool_t alpha = EI_TRUE;

	/* On positionne le pointeur au départ (x1, y1) */
	pixel_ptr += x1 + (y1*width);

	if (x1 == x2) { // Ligne verticale
		int dy = y2 - y1;
		incr = width;
		if (dy < 0) { // Parcours des pixels à l'envers
			dy = -dy;
			incr = -incr;
			sign = -1;
		}
		for (i = 0; i <= dy; i++) {
			draw_pixel(surface, pixel_ptr, x1, y1 + (sign * i), &color, clipper, alpha);
			pixel_ptr += incr; // y += 1
		}
	} else { // Ligne horizontale
		int dx = x2 - x1;
		incr = 1;
		sign = 1;
		if (dx < 0) { // Parcours des pixels à l'envers
			dx = -dx;
			incr = -incr;
			sign = -1;
		}
		for (i = 0; i <= dx; i++) {
			draw_pixel(surface, pixel_ptr, x1 + (sign * i), y1, &color, clipper, alpha);
			pixel_ptr += incr; // x += 1
		}
	}
}

void draw_segment_bresenham(ei_surface_t surface,
			    int x1, int y1, int dx, int dy, int sign_x, int sign_y, int swap,
			    ei_color_t color,
			    const ei_rect_t *clipper) {
	int width = hw_surface_get_size(surface).width;
	int i, j = 0;
	int incr_x = sign_x, incr_y = (sign_y) * width; // Parcours des pixels à l'endroit ou non
	int E = 0;
	uint32_t *pixel_ptr = (uint32_t *) hw_surface_get_buffer(surface);
	ei_bool_t alpha = EI_TRUE;

	/* On positionne le pointeur au départ (x1, y1) */
	pixel_ptr += x1 + (y1*width);

	if (swap == 0) {
		for (i = 0; i <= dx; i++) {
			draw_pixel(surface, pixel_ptr, x1 + (sign_x * i), y1 + (sign_y * j), &color, clipper, alpha);
			pixel_ptr += incr_x; // x+= 1
			E += dy;
			if (2 * E > dx) {
				j++;
				pixel_ptr += incr_y; // y+= 1
				E -= dx;
			}
		}
	} else { // On inverse x et y
		for (i = 0; i <= dy; i++) {
			draw_pixel(surface, pixel_ptr, x1 + (sign_x * j), y1 + (sign_y * i), &color, clipper, alpha);
			pixel_ptr += incr_y; // y+= 1 (swap)
			E += dx;
			if (2 * E > dy) {
				j++;
				pixel_ptr += incr_x; // x+= 1 (swap)
				E -= dy;
			}
		}
	}
}

ei_side_table construct_side_table(ei_surface_t surface, const ei_linked_point_t *first_point) {
	int i, x1, x2, y1, y2, tmp, height = hw_surface_get_size(surface).height;
	const ei_linked_point_t *ptr;
	ei_side **array = malloc(height * sizeof(ei_side*));
	for (i = 0; i < height; i++) {
		array[i] = NULL;
	}
	ei_side_table tc = {0, array};
	for (ptr = first_point; ptr != NULL && ptr->next != NULL; ptr = ptr->next) {
		x1 = ptr->point.x;
		y1 = ptr->point.y;
		x2 = ptr->next->point.x;
		y2 = ptr->next->point.y;
		if (y1 == y2) { // On ignore les côtés horizontaux
			continue;
		}
		if (y1 > y2) { // Point 1 correspond au ymin
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			tmp = x1;
			x1 = x2;
			x2 = tmp;
		}
		if (y1 > height || y1 < 0) {
			continue;
		}
		ei_side *side = malloc(sizeof(ei_side));
		side->ymax = y2;
		side->x_ymin = x1;
		side->dx = x2 - x1;
		side->dy = y2 - y1; // Notons que dy est toujours positif
		side->E = 0;
		if (tc.array[y1] == NULL) {
			side->next = NULL;
			tc.array[y1] = side;
		} else {
			side->next = tc.array[y1];
			tc.array[y1] = side;
		}
		tc.length++;
	}
	return tc;
}

void move_sides_to_tca(ei_side_table *tc, int y, ei_side **tca) {
	if (tc->array[y] != NULL) {
		if ((*tca) == NULL) {
			*tca = tc->array[y];
		} else {
			ei_side *ptr = *tca;
			while (ptr->next != NULL) {
				ptr = ptr->next;
			}
			ptr->next = tc->array[y];
		}
		while (tc->array[y] != NULL) { // vider tc->array[y]
			tc->array[y] = tc->array[y]->next; // ne pas free, car déplacé dans tca
			tc->length--;
		}
		tc->array[y] = NULL;
	}
}

void delete_ymax_from_tca(ei_side **tca, int y) {
	ei_side sent = {0, 0, 0, 0, 0, *tca};
	ei_side *ptr = &sent;
	while (ptr != NULL && ptr->next != NULL) {
		if (ptr->next->ymax == y) {
			ei_side *to_delete = ptr->next;
			ptr->next = to_delete->next;
			free(to_delete);
		} else {
			ptr = ptr->next;
		}
	}
	*tca = sent.next;
}

void swap_sides(ei_side *s1, ei_side *s2) {
	int tmp1, tmp2, tmp3, tmp4, tmp5;
	tmp1 = s1->ymax;
	tmp2 = s1->x_ymin;
	tmp3 = s1->dx;
	tmp4 = s1->dy;
	tmp5 = s1->E;

	s1->ymax = s2->ymax;
	s1->x_ymin = s2->x_ymin;
	s1->dx = s2->dx;
	s1->dy = s2->dy;
	s1->E = s2->E;

	s2->ymax = tmp1;
	s2->x_ymin = tmp2;
	s2->dx = tmp3;
	s2->dy = tmp4;
	s2->E = tmp5;
}

void sort_side_table(ei_side *side) {
	ei_side *i, *j, *min;
	for (i = side; i != NULL && i->next != NULL; i = i->next) {
		min = i;
		for (j = i->next; j != NULL; j = j->next) {
			if (j->x_ymin < min->x_ymin) {
				min = j;
			}
		}
		if (min != i) { // Swap min and i
			swap_sides(min, i);
		}
	}
}

void draw_scanline(ei_surface_t surface, uint32_t **pixel_ptr, ei_side *tca, int y, ei_color_t color,
		   const ei_rect_t *clipper) {
	int drawing = 0, x;
	ei_bool_t alpha = EI_TRUE;
	ei_side sent = {0, 0, 0, 0, 0, tca};
	ei_side *ptr;
	for (ptr = &sent; ptr != NULL && ptr->next != NULL; ptr = ptr->next) {
		if (drawing) {
			for (x = ptr->x_ymin; x < ptr->next->x_ymin; x++) {
				/* TODO: arrondi de la condition de remplissage (sûrement avec E) */
				draw_pixel(surface, *pixel_ptr, x, y, &color, clipper, alpha);
				*pixel_ptr += 1;
			}
			drawing = 0;
		} else {
			*pixel_ptr += (ptr->next->x_ymin) - (ptr->x_ymin);
			drawing = 1;
		}
	}
	*pixel_ptr += hw_surface_get_size(surface).width - ptr->x_ymin; // pixel_ptr sur la prochaine scanline
}

ei_point_t find_intersection(int y, ei_side *side) {
	/* TODO: Correction possible si jamais "remplissage de quelques pixels en moins" */
	/* --> lorsque plusieurs pixels sur scanline y, sélectionner "le bon" (voir schéma) */

	ei_point_t point;
	int x = side->x_ymin;

	if (abs(side->dx) <= side->dy) { // |dx| <= |dy| (side->dy toujours positif)
		if (side->dx > 0) { // dx > 0
			side->E += side->dx;
			if (2 * side->E > side->dy) {
				side->E -= side->dy;
				x++;
			}
		} else if (side->dx < 0){ // dx < 0
			side->E += (-side->dx);
			if (2 * side->E > side->dy) {
				side->E -= side->dy;
				x--;
			}
		} // rien si dx == 0
	} else { // |dx| > |dy|
		if (side->dx > 0) { // dx > 0
			do {
				x++;
				side->E += side->dy;
			} while (2 * side->E <= side->dx);
			side->E -= side->dx;
		} else { // dx < 0
			do {
				x--;
				side->E += side->dy;
			} while (2 * side->E <= (-side->dx));
			side->E -= (-side->dx);
		}
	}
	point.x = x;
	point.y = y;
	return point;
}


void update_scanline(ei_side *tca, int y) {
	ei_side *ptr;
	ei_point_t intersect;
	for (ptr = tca; ptr != NULL; ptr = ptr->next) {
		// On veut trouver l'intersection entre le côté et la scanline
		// x_ymin sera l'abscisse (nouvelle) de cette intersection
		intersect = find_intersection(y, ptr);
		ptr->x_ymin = intersect.x;
	}
}
