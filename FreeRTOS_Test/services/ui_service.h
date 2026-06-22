#ifndef UI_SERVICE_H
#define UI_SERVICE_H
#include <stdbool.h>

typedef enum
{
    UI_MODE_CLOCK = 0,
    UI_MODE_SET_TIME,
    UI_MODE_SET_ALARM,
    UI_MODE_DISPLAY_OFF

} ui_mode_t;



/*
 * Initialize UI state
 */
void ui_service_init(void);

/*
 * Advance to next mode
 */
void ui_service_next_mode(void);

/*
 * Set mode directly
 */
void ui_service_set_mode(ui_mode_t mode);

/*
 * Get current mode
 */
ui_mode_t ui_service_get_mode(void);

/*
 * Set whether hour or minute is being adjusted in time/alarm set modes
 */
void ui_service_set_hour_mode(bool hour_mode);
void ui_service_toggle_hour_mode(void);
bool ui_service_get_hour_mode(void);

#endif