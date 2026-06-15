#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

void display_init(void);

void display_show_time(
    int hours,
    int minutes,
    int blink_hours,
    int blink_minutes
);

#endif