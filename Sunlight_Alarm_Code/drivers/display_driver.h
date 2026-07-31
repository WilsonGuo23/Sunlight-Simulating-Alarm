#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

void display_driver_init(uint8_t clk_pin, uint8_t dio_pin);
void display_driver_show_time(uint8_t hours, uint8_t minutes, bool hour_min_mode, bool flicker);
void display_driver_clear(void);

#endif
