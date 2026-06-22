#include <stdio.h>
#include "pico/stdlib.h"
#include <time.h>
#include <string.h>

#include "config.h"
#include "display.h"
#include "time_manager.h"
#include "buttons.h"
#include "dimmer.h"


#include "alarm_manager.h"
// ========================================
// GLOBAL VARIABLES
// ========================================

uint64_t last_clock_update = 0;
uint64_t last_display_update = 0;

struct tm* clock_time;


// ========================================
// LIGHT CONTROL
// ========================================

// Used only by display update logic now
static bool flicker = false;



// Light control moved to dimmer module

// ========================================
// ALARM FUNCTIONS moved to alarm_manager.c

// ========================================
// INITIALIZATION
// ========================================

static void master_init()
{
    stdio_init_all();

    time_manager_init();
    clock_time = time_manager_get_time();
    gpio_init_mask(BIT_MASK);
    // Initialize buttons (IRQ setup, input direction, pull-ups)
    buttons_init();

    // Zero-cross and triac pins
    gpio_set_irq_enabled(ZERO_CROSS_PIN, GPIO_IRQ_EDGE_FALL, true);

    gpio_set_dir(ZERO_CROSS_PIN, GPIO_IN);
    gpio_set_dir(TRIAC_PIN, GPIO_OUT);

    alarm_manager_init(clock_time);
}

// ========================================
// MAIN
// ========================================

int main()
{
    master_init();

    display_init();

    while(true)
    {
        uint64_t now = time_us_64();

        if(change_mode_flag)
        {
            change_mode();
            change_mode_flag = false;
        }

        if(increment_flag)
        {
            increment_column();
            increment_flag = false;
        }

        if(decrement_flag)
        {
            decrement_column();
            decrement_flag = false;
        }

        if(move_flag)
        {
            move_column();
            move_flag = false;
        }

        if(snooze_flag)
        {
            alarm_on = false;
            snooze_flag = false;

            dimmer_reset_delay();
            gpio_set_irq_enabled(
                ZERO_CROSS_PIN,
                GPIO_IRQ_EDGE_FALL,
                false);
            gpio_put(TRIAC_PIN, 0);
        }

        if(now - last_display_update >= 500000)
        {
            last_display_update = now;

            bool blink_hours = false;
            bool blink_minutes = false;

            if(current_mode != RESTING)
            {
                flicker = !flicker;

                if(minutes_selected)
                {
                    blink_minutes = !flicker;
                }
                else
                {
                    blink_hours = !flicker;
                }
            }

            if(current_mode == MODE_SET_ALARM_TIME)
            {
                display_show_time(
                    alarm_time_hours,
                    alarm_time_minutes,
                    blink_hours,
                    blink_minutes);
            }
            else
            {
                display_show_time(
                    clock_time->tm_hour,
                    clock_time->tm_min,
                    blink_hours,
                    blink_minutes);
            }
        }

        if(now - last_clock_update >= 1000000)
        {
            last_clock_update = now;

            clock_increment();

            alarm_check();
        }

        dimmer_update(now, alarm_on);
    }
}