#ifndef DIMMER_H
#define DIMMER_H

#include <stdint.h>
#include <stdbool.h>

void dimmer_init(void);
void dimmer_update(uint64_t now, bool alarm_on);
void dimmer_reset_delay(void);
void zero_cross_callback(uint32_t gpio, uint32_t events);

#endif
