/**
 *  @file	ei_widgetclass_utils.h
 *  @brief	Functions that are useful to \ref ei_widgetclass.h
 *
 */


#ifndef EI_WIDGETCLASS_UTILS_H
#define EI_WIDGETCLASS_UTILS_H

#include "directory.h"

/**
 * \brief	Returns the global variable of the dictionnary of widget classes
 *
 * @return 		Dictionnary of widget classes
 */
struct dir *get_widget_dir(void);

/**
 * \brief 	Frees the dictionnary of widget classes
 */
void free_widget_dir(void);

/**
 * \brief	Sets the first widget class (used for freeing every widget class)
 *
 * @param	wclass
 */
void set_first_widgetclass(ei_widgetclass_t *wclass);

#endif //EI_WIDGETCLASS_UTILS_H
