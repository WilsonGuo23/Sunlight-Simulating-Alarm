#include "display_service.h"
#include "stdio.h"

//initialization
void display_service_init(void)
{
    display_driver_show_time(0, 0, true, false);
}

//shows the time on the display by calling the display driver, hour_min_mode is only relevant if flicker is on. 
//flicker tells the display to turn off the appropriate side to make it apparent which side is interactable
void display_service_show_time(uint8_t hour,uint8_t minute, bool hour_min_mode, bool flicker)
{
    display_driver_show_time(hour, minute, hour_min_mode, flicker);
}

//clears tm1637 display
void display_service_clear(void)
{
    display_driver_clear();
}

