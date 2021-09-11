#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_draw.h"
#include "ei_types.h"
#include "ei_event.h"
#include "ei_button.h"
#include "ei_widget_utils.h"



/* test_line --
 *
 *	Draws a simple line in the canonical octant, that is, x1>x0 and y1>y0, with
 *	dx > dy. This can be used to test a first implementation of Bresenham
 *	algorithm, for instance.
 */
void test_line(ei_surface_t surface, ei_rect_t* clipper)
{
	ei_color_t		color		= { 255, 0, 255, 230 };
	ei_linked_point_t	pts[2];

	pts[0].point.x = 200; pts[0].point.y = 200; pts[0].next = &pts[1];
	pts[1].point.x = 600; pts[1].point.y = 400; pts[1].next = NULL;

	ei_draw_polyline(surface, pts, color, clipper);
}



/* test_octogone --
 *
 *	Draws an octogone in the middle of the screen. This is meant to test the
 *	algorithm that draws a polyline in each of the possible octants, that is,
 *	in each quadrant with dx>dy (canonical) and dy>dx (steep).
 */
void test_octogone(ei_surface_t surface, ei_rect_t* clipper, int polygon)
{
	ei_color_t		color		= { 255, 100, 255, 255 };
	ei_linked_point_t	pts[9];
	int			i, xdiff, ydiff;

	/* Initialisation */
	pts[0].point.x = 400;
	pts[0].point.y = 90;

	/* Draw the octogone */
	for(i = 1; i <= 8; i++) {
		 /*	Add or remove 70/140 pixels for next point
			The first term of this formula gives the sign + or - of the operation
			The second term is 2 or 1, according to which coordinate grows faster
			The third term is simply the amount of pixels to skip */
		xdiff = pow(-1, (i + 1) / 4) * pow(2, (i / 2) % 2 == 0) * 70;
		ydiff = pow(-1, (i - 1) / 4) * pow(2, (i / 2) % 2) * 70;

		pts[i].point.x = pts[i-1].point.x + xdiff;
		pts[i].point.y = pts[i-1].point.y + ydiff;
		pts[i-1].next = &(pts[i]);
	}

	/* End the linked list */
	pts[i-1].next = NULL;

	if (polygon) {
		ei_draw_polygon(surface, pts, color, clipper);
	} else {
		ei_draw_polyline(surface, pts, color, clipper);
	}
}



/* test_square --
 *
 *	Draws a square in the middle of the screen. This is meant to test the
 *	algorithm for the special cases of horizontal and vertical lines, where
 *	dx or dy are zero
 */
void test_square(ei_surface_t surface, ei_rect_t* clipper, int polygon)
{
	ei_color_t		color		= { 255, 255, 0, 255 };
	ei_linked_point_t	pts[5];
	int			i, xdiff, ydiff;

	/* Initialisation */
	pts[0].point.x = 300;
	pts[0].point.y = 400;

	/* Draw the square */
	for(i = 1; i <= 4; i++) {
		/*	Add or remove 200 pixels or 0 for next point
			The first term of this formula gives the sign + or - of the operation
			The second term is 0 or 1, according to which coordinate grows
			The third term is simply the side of the square */
		xdiff = pow(-1, i / 2) * (i % 2) * 200;
		ydiff = pow(-1, i / 2) * (i % 2 == 0) * 200;

		pts[i].point.x = pts[i-1].point.x + xdiff;
		pts[i].point.y = pts[i-1].point.y + ydiff;
		pts[i-1].next = &(pts[i]);
	}

	/* End the linked list */
	pts[i-1].next = NULL;

	if (polygon) {
		ei_draw_polygon(surface, pts, color, clipper);
	} else {
		ei_draw_polyline(surface, pts, color, clipper);
	}
}



/* test_dot --
 *
 *	Draws a dot in the middle of the screen. This is meant to test the special 
 *	case when dx = dy = 0
 */
void test_dot(ei_surface_t surface, ei_rect_t* clipper)
{
	ei_color_t		color		= { 0, 0, 0, 255 };
	ei_linked_point_t	pts[3];

	pts[0].point.x = 400; pts[0].point.y = 300; pts[0].next = &(pts[1]);
	pts[1].point.x = 400; pts[1].point.y = 300; pts[1].next = NULL;

	ei_draw_polyline(surface, pts, color, clipper);
}

void test_triangle(ei_surface_t surface, ei_rect_t* clipper, int polygon) {
        ei_color_t              color           = {255, 0, 0, 120};
        ei_linked_point_t       pts[4];

        pts[0].point.x = 540; pts[0].point.y = 160; pts[0].next = &(pts[1]);
        pts[1].point.x = 400; pts[1].point.y = 510; pts[1].next = &(pts[2]);
        pts[2].point.x = 260; pts[2].point.y = 160; pts[2].next = &(pts[3]);
        pts[3].point.x = 540; pts[3].point.y = 160; pts[3].next = NULL;

        if (polygon) {
                ei_draw_polygon(surface, pts, color, clipper);
        } else {
                ei_draw_polyline(surface, pts, color, clipper);
        }
}

/* test_arc --
 *
 */
void test_arc(ei_surface_t surface, ei_rect_t* clipper) {
        ei_color_t		color		= { 255, 0, 0, 50 };
        ei_point_t centre;
        centre.x = 400; centre.y = 300;
        float rayon = 200; float debut = 0; float fin = 2*M_PI;
        ei_linked_point_t *pts = arc(centre, rayon, debut, fin);
        ei_draw_polygon(surface, pts, color, clipper);
}

void test_rounded_frame	(ei_surface_t surface, ei_rect_t *clipper) {
        ei_color_t		color		= { 255, 0, 0, 100 };
        ei_size_t taille; taille.height = 150; taille.width = 150;
        ei_point_t pt_rect; pt_rect.x = 0; pt_rect.y = 0;
        ei_rect_t rect; rect.top_left = pt_rect ; rect.size = taille;
        float rayon = 25;
        ei_linked_point_t *pts = rounded_frame(rect, rayon, 1, 1);
        ei_draw_polygon(surface, pts, color, clipper);
        free_points(pts);
        //ei_color_t bot_color = {0, 255, 0, 255};
        //pts = rounded_frame(rect, rayon, 0, 1);
        //ei_draw_polygon(surface, pts, bot_color, clipper);
        //free_points(pts);
        //rect.top_left.x += rect.size.width/20;
        //rect.top_left.y += rect.size.height/20;
        //rect.size.width -= rect.size.width*2/20;
        //rect.size.height -= rect.size.width*2/20;
        //pts = rounded_frame(rect, rayon, 1, 1);
        //ei_color_t inside_color = {0,0,255,255};
        //ei_draw_polygon(surface, pts, inside_color, clipper);
        //free_points(pts);
}

void test_text(ei_surface_t surface, ei_rect_t *clipper) {
        ei_point_t where; where.x = 200; where.y = 200;
        char *text = "hello";
        ei_font_t font = ei_default_font;
        ei_color_t color = {255, 0, 0, 255};
        ei_draw_text(surface, &where, text, font, color, clipper);
}

void test_button(ei_surface_t surface, ei_rect_t *clipper) {
        const char *text = "button";
        ei_font_t font = ei_default_font;
        ei_color_t text_color = {255, 255, 255, 255};
        float rayon = 25;
        ei_color_t inside_color = {100, 100, 100, 255};
        ei_size_t taille; taille.height = 100; taille.width = 100;
        ei_point_t pt_rect; pt_rect.x = 200; pt_rect.y = 200;
        ei_rect_t rect; rect.top_left = pt_rect ; rect.size = taille;
        ei_relief_t relief = ei_relief_sunken;
        draw_button(surface, text, font, text_color, clipper,
                    rect, inside_color, rayon, relief, EI_FALSE);
}

void test_toplevel (ei_surface_t surface, ei_rect_t *clipper) {
        const char *text = "Toplevel";
        ei_font_t font = ei_default_font;
        ei_color_t text_color = {255, 255, 255, 50};
        ei_color_t inside_color = {100, 100, 100, 50};
        ei_size_t taille; taille.height = 200; taille.width = 500;
        ei_point_t pt_rect; pt_rect.x = 200; pt_rect.y = 400;
        ei_rect_t rect; rect.top_left = pt_rect ; rect.size = taille;
        draw_toplevel(surface, text, font, text_color, clipper,
                    rect, inside_color, EI_FALSE, 5);
}

/*
 * ei_main --
 *
 *	Main function of the application.
 */
int main(int argc, char** argv)
{
	ei_size_t		win_size	= ei_size(800, 600);
	ei_surface_t		main_window	= NULL;
	ei_color_t		white		= { 0, 153, 255, 0xff };
	ei_rect_t*		clipper_ptr	= NULL;
	ei_rect_t		clipper		= ei_rect(ei_point(0, 250), ei_size(800, 100));
	clipper_ptr		= &clipper;
	clipper_ptr		= NULL;
	ei_event_t		event;

	hw_init();
		
	main_window = hw_create_window(win_size, EI_FALSE);
	
	/* Lock the drawing surface, paint it white. */
	hw_surface_lock	(main_window);
	ei_fill		(main_window, &white, clipper_ptr);

	/* Draw polylines. */
	test_line	(main_window, clipper_ptr);
	test_octogone	(main_window, clipper_ptr, 0);
	test_square	(main_window, clipper_ptr, 0);
	test_triangle(main_window, clipper_ptr, 1);

        /* arc. */
	test_arc	(main_window, clipper_ptr);

        /* rounded_frame. */
      	test_rounded_frame	(main_window, clipper_ptr);
        test_button             (main_window, clipper_ptr);
        test_toplevel           (main_window, clipper_ptr);

        ei_rect_t dst = ei_rect(ei_point(500, 0), ei_size(300, 200));
        ei_rect_t src = ei_rect(ei_point(200, 200), ei_size(300, 200));
	ei_copy_surface(main_window, &dst, main_window, &src, EI_FALSE);

	/* Unlock and update the surface. */
	hw_surface_unlock(main_window);
	hw_surface_update_rects(main_window, NULL);
	
	/* Wait for a character on command line. */
	event.type = ei_ev_none;
	while (event.type != ei_ev_keydown)
		hw_event_wait_next(&event);

	hw_quit();
	return (EXIT_SUCCESS);
}
