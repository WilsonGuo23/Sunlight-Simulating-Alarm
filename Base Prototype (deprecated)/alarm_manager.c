#include "alarm_manager.h"
#include "config.h"
#include "pico/stdlib.h"
#include <time.h>

int alarm_time_minutes = 0;
int alarm_time_hours = 0;
int minutes_selected = 1;
int current_mode;
bool alarm_on = false;
volatile alarm_id_t timing_delay = -1;

static struct tm* clock_time = NULL;

void alarm_manager_init(struct tm* clock_time_ptr)
{
    clock_time = clock_time_ptr;
}

void change_mode(void)
{
    switch(current_mode)
    {
        case MODE_SET_CURRENT_TIME:
            current_mode++;
            break;

        case MODE_SET_ALARM_TIME:
            current_mode++;
            break;

        case RESTING:
            current_mode = 0;
            break;
    }
}

void increment_column(void)
{
    if(current_mode == MODE_SET_ALARM_TIME)
    {
        if(minutes_selected)
        {
            alarm_time_minutes++;

            if(alarm_time_minutes >= 60)
                alarm_time_minutes = 0;
        }
        else
        {
            alarm_time_hours++;

            if(alarm_time_hours >= 24)
                alarm_time_hours = 0;
        }
    }

    if(current_mode == MODE_SET_CURRENT_TIME)
    {
        if(minutes_selected)
        {
            clock_time->tm_min++;

            if(clock_time->tm_min >= 60)
                clock_time->tm_min = 0;
        }
        else
        {
            clock_time->tm_hour++;

            if(clock_time->tm_hour >= 24)
                clock_time->tm_hour = 0;
        }
    }
}

void decrement_column(void)
{
    if(current_mode == MODE_SET_ALARM_TIME)
    {
        if(minutes_selected)
        {
            alarm_time_minutes--;

            if(alarm_time_minutes < 0)
                alarm_time_minutes = 59;
        }
        else
        {
            alarm_time_hours--;

            if(alarm_time_hours < 0)
                alarm_time_hours = 23;
        }
    }

    if(current_mode == MODE_SET_CURRENT_TIME)
    {
        if(minutes_selected)
        {
            clock_time->tm_min--;

            if(clock_time->tm_min < 0)
                clock_time->tm_min = 59;
        }
        else
        {
            clock_time->tm_hour--;

            if(clock_time->tm_hour < 0)
                clock_time->tm_hour = 23;
        }
    }
}

void move_column(void)
{
    minutes_selected = !minutes_selected;
}

void alarm_check(void)
{
    if(alarm_time_hours == clock_time->tm_hour &&
       alarm_time_minutes == clock_time->tm_min)
    {
        alarm_on = true;

        gpio_set_irq_enabled(
            ZERO_CROSS_PIN,
            GPIO_IRQ_EDGE_FALL,
            true);
    }
}
