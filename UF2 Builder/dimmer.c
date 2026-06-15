#include "dimmer.h"
#include "config.h"
#include "pico/stdlib.h"
#include <stdio.h>

#include <stdint.h>
#include <stdbool.h>

static bool end_of_cycle = false;
volatile uint delay_us = 6000; // 2000-6000
static uint64_t last_delay_decrement_update = 0;
static volatile uint64_t last_zero_cross_time = 0;

static void decrement_delay(void)
{
    if (!end_of_cycle)
    {
        delay_us -= 100;
    }

    if (delay_us <= 2000)
    {
        end_of_cycle = true;
    }
}

int64_t triac_fire_callback_callback_end(alarm_id_t id, void *user_data)
{
    //printf("TRIAC turning OFF\n");
    gpio_put(TRIAC_PIN, 0);
    return 0;
}

int64_t triac_fire_callback(alarm_id_t id, void *user_data)
{
    //printf("TRIAC FIRING! Setting TRIAC_PIN to 1 (and keeping it on)\n");
    gpio_put(TRIAC_PIN, 1);

    return 0;
}

// Handle the AC zero-cross interrupt by scheduling lighting control based on the current delay.
void zero_cross_callback(uint32_t gpio, uint32_t events)
{
    uint64_t now = time_us_64();

    if(now - last_zero_cross_time < 5000)
        return;

    last_zero_cross_time = now;

    //printf("Zero-cross detected, scheduling fire in %d us\n", delay_us);

    add_alarm_in_us(
        delay_us,
        triac_fire_callback,
        NULL,
        true);
}

void dimmer_init(void)
{
    // nothing for now; pins set by master_init
}

void dimmer_update(uint64_t now, bool alarm_on)
{
    if(now - last_delay_decrement_update >= 10000 && alarm_on)
    {
        last_delay_decrement_update = now;
        decrement_delay();
        //printf("Dimmer update: delay_us=%d, end_of_cycle=%d\n", delay_us, end_of_cycle);
    }
}

void dimmer_reset_delay(void)
{
    delay_us = 6000;
    end_of_cycle = false;
    last_delay_decrement_update = 0;
}
