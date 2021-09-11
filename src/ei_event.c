#include <stdlib.h>

#include "ei_event.h"
#include "ei_widget.h"

/** Global variables **/
/**                  **/
ei_default_handle_func_t DEFAULT_HANDLE_FUNC = NULL;
ei_widget_t *ACTIVE_WIDGET = NULL;
/**                  **/
/** ---------------- **/


/**
 * Sets the widget which is currently being manipulated by the user.
 *
 * @param	widget		The widget to declare as active, or NULL to declare
 *				that a widget is no more being manipulated.
 */
void ei_event_set_active_widget(ei_widget_t *widget) {
	ACTIVE_WIDGET = widget;
}

/**
 * Returns the widget currently being manipulated by the user.
 *
 * @return			The widget currenlty being manipulated, or NULL.
 */
ei_widget_t *ei_event_get_active_widget(void) {
	return ACTIVE_WIDGET;
}

/**
 * Sets the function that must be called when an event has been received but no processed
 *	by any widget.
 *
 * @param	func		The event handling function.
 */
void ei_event_set_default_handle_func(ei_default_handle_func_t func) {
	DEFAULT_HANDLE_FUNC = func;
}

/**
 * Returns the function that must be called when an event has been received but no processed
 *	by any widget.
 *
 * @return			The address of the event handling function.
 */
ei_default_handle_func_t ei_event_get_default_handle_func(void) {
	return DEFAULT_HANDLE_FUNC;
}