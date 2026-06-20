#include "display_service.h"
#include "display_driver.h"
#include "stdio.h"

void display_service_init(void)
{
    display_driver_show_time(0, 0);
    //display_driver_set_brightness(100);
}

void display_service_show_time(uint8_t hour,
                               uint8_t minute)
{
    display_driver_show_time(hour, minute);
}

