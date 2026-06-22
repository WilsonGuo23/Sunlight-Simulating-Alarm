#ifndef BUTTONS_DRIVER_H
#define BUTTONS_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    BUTTON_NONE,
    BUTTON_INC,
    BUTTON_DEC,
    BUTTON_MODE,
    BUTTON_SNOOZE,
    BUTTON_HOUR_MIN
} button_t;

void buttons_init(uint8_t inc_pin,
                  uint8_t dec_pin,
                  uint8_t mode_pin,
                  uint8_t snooze_pin,
                  uint8_t hour_min_pin);

bool buttons_read(button_t *button);

static void button_to_string(button_t *button);
#endif