#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

#include <stdint.h>
#include "display_driver.h"
#include <stdbool.h>

void display_service_init(void);

void display_service_show_time(uint8_t hour,
                               uint8_t minute,
                               bool hour_min_mode,
                               bool flicker);

void display_service_clear(void);

#endif