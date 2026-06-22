#ifndef SUNRISE_SERVICE_H
#define SUNRISE_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

void sunrise_service_init(void);

/*
 * Start a sunrise ramp
 */
void sunrise_service_start(void);

/*
 * Stop sunrise and turn light off
 */
void sunrise_service_stop(void);

/*
 * Called periodically by a task
 */
void sunrise_service_update(void);

bool sunrise_service_is_active(void);

uint8_t sunrise_service_get_brightness(void);

#endif