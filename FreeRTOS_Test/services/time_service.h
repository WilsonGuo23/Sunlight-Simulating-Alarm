#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

#include <stdint.h>

void time_service_init(void);

void time_service_update(void);

void time_service_set(uint8_t hour,
                      uint8_t minute);

void time_service_get(uint8_t *hour,
                      uint8_t *minute);

void time_service_increment_hour(void);

void time_service_increment_minute(void);

void time_task(void *pvParameters);
#endif