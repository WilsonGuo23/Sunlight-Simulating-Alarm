#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <stdint.h>

void display_driver_init(void);
void display_driver_show_time(uint8_t hours, uint8_t minutes, uint8_t seconds);

#endif
