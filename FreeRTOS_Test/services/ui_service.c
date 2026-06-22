#include "ui_service.h"


static ui_mode_t current_mode_;
static bool hour_mode_;

void ui_service_init(void)
{
    current_mode_ = UI_MODE_CLOCK;
    hour_mode_ = true;
}

void ui_service_next_mode(void)
{
    switch (current_mode_)
    {
        case UI_MODE_CLOCK:
            current_mode_ = UI_MODE_SET_TIME;
            break;

        case UI_MODE_SET_TIME:
            current_mode_ = UI_MODE_SET_ALARM;
            break;

        case UI_MODE_SET_ALARM:
            current_mode_ = UI_MODE_DISPLAY_OFF;
            break;

        case UI_MODE_DISPLAY_OFF:
            current_mode_ = UI_MODE_CLOCK;
            break;

        default:
            current_mode_ = UI_MODE_CLOCK;
            break;
    }
}

void ui_service_set_mode(ui_mode_t mode)
{
    current_mode_ = mode;
}

ui_mode_t ui_service_get_mode(void)
{
    return current_mode_;
}

void ui_service_set_hour_mode(bool hour_mode)
{
    hour_mode_ = hour_mode;
}

void ui_service_toggle_hour_mode(void)
{
    if (hour_mode_ == true)
    {
        hour_mode_ = false;
    }
    else
    {
        hour_mode_ = true;
    }
}

bool ui_service_get_hour_mode(void)
{
    return hour_mode_;
}

