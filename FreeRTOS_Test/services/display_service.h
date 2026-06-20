#ifndef DISPLAY_SERVICE_H
#define DISPLAY_SERVICE_H

#include <stdint.h>

void display_service_init(void);

void display_service_show_time(uint8_t hour,
                               uint8_t minute);

#endif