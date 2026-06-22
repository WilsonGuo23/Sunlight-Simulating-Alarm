#ifndef DRIVER_TRIAC_H
#define DRIVER_TRIAC_H

#include <stdint.h>
#include <stdbool.h>

void triac_init(uint8_t triac_pin,
                uint8_t zero_cross_pin);

void triac_set_brightness(uint8_t percent);

uint8_t triac_get_brightness(void);

#endif