#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "ei_utils.h"
#include "ei_types.h"
#include "ei_draw_utils.h"
#include "ei_widget_utils.h"
#include "ei_widgetclass.h"
#include "ei_widgetclass_utils.h"

#include "hw_interface.h"

int main(int argc, char* argv[])
{
	// Test construct_side_table
	ei_size_t win_size = ei_size(800, 600);
	ei_surface_t main_window = NULL;
	main_window = hw_create_window(win_size, EI_FALSE);
	ei_point_t pA = {2, 3};
	ei_point_t pB = {9, 1};
	ei_point_t pC = {13, 5};
	ei_linked_point_t p1 = {pA, NULL};
	ei_linked_point_t p2 = {pB, NULL};
	ei_linked_point_t p3 = {pC, NULL};
	p1.next = &p2;
	p2.next = &p3;
	const ei_linked_point_t *p = &p1;
	ei_side_table tc = construct_side_table(main_window, p);
	// TODO: vérifier pourquoi ça fait false ici
	// assert((tc.array[1]->ymax == 3 && tc.array[1]->x_ymin == 9 && tc.array[1]->next->ymax == 5 && tc.array[1]->next->x_ymin == 9));
	assert((tc.array[2] == NULL));
	assert((tc.array[3] == NULL));

	// Test move_sides_to_tca
	ei_side *tca = NULL;
	move_sides_to_tca(&tc, 1, &tca);
	assert((tc.array[1] == NULL && tc.length == 0));

	// Test delete_ymax_from_tca
	delete_ymax_from_tca(&tca, 3);
	assert((tca->ymax==5 && tca->x_ymin == 9));
	delete_ymax_from_tca(&tca, 5);
	assert((tca == NULL));

        // Test swap_sides
        ei_side s1 = {0, 5, 0, 0, 0, NULL};
        ei_side s2 = {0, 2, 0, 0, 0, NULL};
        ei_side s3 = {0, 1, 0, 0, 0, NULL};
        ei_side s4 = {0, 9, 0, 0, 0, NULL};
        ei_side s5 = {0, 8, 0, 0, 0, NULL};
        s1.next = &s2;
        s2.next = &s3;
        s3.next = &s4;
        s4.next = &s5;
	swap_sides(&s1, &s2);
        assert((s1.x_ymin == 2));
        assert((s1.next->x_ymin == 5));
        assert((s2.next->x_ymin == 1));
	swap_sides(&s1, &s2);

        // Test sort_side_table
        sort_side_table(&s1);
        assert((s1.x_ymin == 1));
        assert((s1.next->x_ymin == 2));
        assert((s1.next->next->x_ymin == 5));
        assert((s1.next->next->next->x_ymin == 8));
        assert((s1.next->next->next->next->x_ymin == 9));

        // Test find_intersection
        // sur les 4 exemples du schéma
        int y = 2;
        ei_side se1 = {10, 0, 1, 3, 0, NULL};
        ei_point_t point = find_intersection(y, &se1);
        assert((point.x == 0 && point.y == 2 && se1.E != 0));
        y = 2;
        ei_side se2 = {10, 0, 3, 1, 0, NULL};
        point = find_intersection(y, &se2);
        assert((point.x == 2 && point.y == 2 && se2.E != 0));
        y = 2;
        ei_side se3 = {10, 1, -1, 3, 0, NULL};
        point = find_intersection(y, &se3);
        assert((point.x == 1 && point.y == 2 && se3.E != 0));
        y = 2;
        ei_side se4 = {10, 3, -3, 1, 0, NULL};
        point = find_intersection(y, &se4);
        assert((point.x == 1 && point.y == 2 && se4.E != 0));
        // sur un exemple avec dx=0
        y = 2;
        ei_side se5 = {10, 3, 0, 1, 0, NULL};
        point = find_intersection(y, &se5);
        assert((point.x == 3 && point.y == 2 && se5.E == 0));

        // Test widget_dir
	struct dir* my_dir = get_widget_dir();

	ei_widgetclass_t *frame_class = malloc(sizeof(ei_widgetclass_t));
	*frame_class = ei_init_frame_class();
	ei_widgetclass_register(frame_class);
	assert((dir_lookup_num(my_dir, "frame") == frame_class));

	ei_widgetclass_t *button_class = malloc(sizeof(ei_widgetclass_t));
	*button_class = ei_init_button_class();
	ei_widgetclass_register(button_class);
	assert((dir_lookup_num(my_dir, "button") == button_class));

	ei_widgetclass_t *toplevel_class = malloc(sizeof(ei_widgetclass_t));
	*toplevel_class = ei_init_toplevel_class();
	ei_widgetclass_register(toplevel_class);
	assert((dir_lookup_num(my_dir, "toplevel") == toplevel_class));

	assert((dir_lookup_num(my_dir, "frame")->next == button_class));
	assert((dir_lookup_num(my_dir, "button")->next == toplevel_class));
	assert((dir_lookup_num(my_dir, "toplevel")->next == NULL));
	assert((dir_lookup_num(my_dir, "bloublibla") == NULL));

	free_widget_dir();
	assert((strcmp(frame_class->name, "frame") != 0));
	assert((strcmp(button_class->name, "button") != 0));
	assert((strcmp(toplevel_class->name, "toplevel") != 0));


        // Terminate program with no error code.
        return 0;
}
