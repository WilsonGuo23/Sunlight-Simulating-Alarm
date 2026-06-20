#ifndef UI_SERVICE_H
#define UI_SERVICE_H

typedef enum
{
    UI_MODE_CLOCK = 0,
    UI_MODE_SET_TIME,
    UI_MODE_SET_ALARM,
    UI_MODE_DISPLAY_OFF

} ui_mode_t;

typedef enum
{
    HOUR_MIN_MODE_HOUR = 0,
    HOUR_MIN_MODE_MINUTE
} hour_min_mode_t;

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

#endif