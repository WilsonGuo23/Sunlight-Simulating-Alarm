#ifndef ALARM_SERVICE_H
#define ALARM_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include "input_service.h"
#include "time_service.h"

void alarm_service_init(void);

void alarm_service_set_alarm(uint8_t hour,
                             uint8_t minute);

void alarm_service_get_alarm(uint8_t *hour,
                             uint8_t *minute);

void alarm_service_enable(bool enable);

bool alarm_service_is_enabled(void);

void alarm_service_update(void);

#endif