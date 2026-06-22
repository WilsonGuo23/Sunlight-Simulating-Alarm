#include "display_service.h"
#include "stdio.h"

void display_service_init(void)
{
    display_driver_show_time(0, 0, true, false);
    //display_driver_set_brightness(100);
}

void display_service_show_time(uint8_t hour,uint8_t minute, bool hour_min_mode, bool flicker)
{
    display_driver_show_time(hour, minute, hour_min_mode, flicker);
}

void display_service_clear(void)
{
    display_driver_clear();
}

