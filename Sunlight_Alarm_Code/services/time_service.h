#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H
#include <stdbool.h>
#include <stdint.h>

void time_service_init(void);

void time_service_update(void);

void time_service_set(uint8_t hour,
                      uint8_t minute);

void time_service_get(uint8_t *hour,
                      uint8_t *minute);

void time_service_increment_hour(void);

void time_service_increment_minute(void);

void time_service_decrement_hour(void);

void time_service_decrement_minute(void);

void time_service_increment(bool hour_mode);

void time_service_decrement(bool hour_mode);

#endif