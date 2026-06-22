#include "time_manager.h"
#include <stdio.h>
#include "pico/stdlib.h"

static time_t display_time;
static struct tm* clock_time;

static bool time_synced = false;

void time_manager_init(void)
{
    display_time = time(NULL);

    clock_time = localtime(&display_time);


}

void clock_increment(void)
{
    clock_time->tm_sec++;

    if(clock_time->tm_sec >= 60)
    {
        clock_time->tm_sec = 0;

        clock_time->tm_min++;

        if(clock_time->tm_min >= 60)
        {
            clock_time->tm_min = 0;

            clock_time->tm_hour++;

            if(clock_time->tm_hour >= 24)
            {
                clock_time->tm_hour = 0;
            }
        }
    }
}

struct tm* time_manager_get_time(void)
{
    return clock_time;
}

void time_manager_set_time(time_t t)
{
    display_time = t;

    clock_time = localtime(&display_time);
}

bool time_manager_is_synced(void)
{
    return time_synced;
}