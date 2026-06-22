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

    alarm_enabled_ = true;
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
    bool sunrise_active_ = sunrise_service_is_active();

    if ((!alarm_enabled_) || sunrise_active_)
    {
        return;
    }

    time_service_get(
        &current_hour,
        &current_minute);

    if ((current_hour == alarm_hour_) &&
        (current_minute == alarm_minute_))
    {
        sunrise_service_start();
    }
}

void alarm_service_increment_hour(void)
{
    alarm_hour_++;

    if (alarm_hour_ >= 24)
    {
        alarm_hour_ = 0;
    }
}

void alarm_service_increment_minute(void)
{
    alarm_minute_++;

    if (alarm_minute_ >= 60)
    {
        alarm_minute_ = 0;
    }
}

void alarm_service_decrement_hour(void)
{
    
    if (alarm_hour_ <= 0)
    {
        alarm_hour_ = 23;
    }
    else
    {
        alarm_hour_--;
    }
}

void alarm_service_decrement_minute(void)
{
    if (alarm_minute_ <= 0)
    {
        alarm_minute_ = 59;
    }
    else
    {
        alarm_minute_--;
    }
}

void alarm_service_increment(bool hour_mode)
{
    if (hour_mode)
    {
        alarm_service_increment_hour();
    }
    else
    {
        alarm_service_increment_minute();
    }
}

void alarm_service_decrement(bool hour_mode)
{
    if (hour_mode)
    {
        alarm_service_decrement_hour();
    }
    else
    {
        alarm_service_decrement_minute();
    }
}