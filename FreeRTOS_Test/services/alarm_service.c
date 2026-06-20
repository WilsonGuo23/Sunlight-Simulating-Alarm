#include "alarm_service.h"

#include "time_service.h"
#include "triac_driver.h"
#include "stdio.h"

static uint8_t alarm_hour_;
static uint8_t alarm_minute_;

static bool alarm_enabled_;


void alarm_service_init(void)
{
    alarm_hour_ = 0;
    alarm_minute_ = 0;

    alarm_enabled_ = false;
}

void alarm_service_set_alarm(uint8_t hour,
                             uint8_t minute)
{
    alarm_hour_ = hour % 24;
    alarm_minute_ = minute % 60;
}

void alarm_service_get_alarm(uint8_t *hour,
                             uint8_t *minute)
{
    if (hour)
    {
        *hour = alarm_hour_;
    }

    if (minute)
    {
        *minute = alarm_minute_;
    }
}

void alarm_service_enable(bool enable)
{
    alarm_enabled_ = enable;
}

bool alarm_service_is_enabled(void)
{
    return alarm_enabled_;
}

void alarm_service_update(void)
{
    uint8_t current_hour;
    uint8_t current_minute;

    if (!alarm_enabled_) //|| sunrise_active_)
    {
        return;
    }

    time_service_get(
        &current_hour,
        &current_minute);

    if ((current_hour == alarm_hour_) &&
        (current_minute == alarm_minute_))
    {
        //sunrise_active_ = true;
        printf("the sun should be rising now\n");
        //sunrise_service_start();
    }
}

