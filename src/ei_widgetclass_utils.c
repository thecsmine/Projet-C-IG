#include <stdlib.h>

#include "ei_widgetclass_utils.h"
#include "directory.h"


/** Global variables **/
/**                  **/
ei_widgetclass_t *first_class = NULL;
struct dir* widget_dir = NULL;
/**                  **/
/** ---------------- **/

struct dir* get_widget_dir(void) {
	if (widget_dir == NULL) {
		widget_dir = dir_create(10);
	}
	return widget_dir;
}

void  free_widget_dir(void) {
	ei_widgetclass_t *to_free;
	dir_free(widget_dir);
	while (first_class != NULL) {
		to_free = first_class;
		first_class = first_class->next;
		free(to_free);
	}
}

void set_first_widgetclass(ei_widgetclass_t *widgetclass) {
	first_class = widgetclass;
}