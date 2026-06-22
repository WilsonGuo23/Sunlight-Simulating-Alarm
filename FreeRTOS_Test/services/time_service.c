#include "time_service.h"

#include "FreeRTOS.h"
#include "task.h"

static uint8_t current_hour_;
static uint8_t current_minute_;

//sets time to 0:0
void time_service_init(void)
{
    current_hour_ = 0;
    current_minute_ = 0;
}

//increments minute and handles number base conversion, called once per minute
void time_service_update(void)
{
    current_minute_++;

    if (current_minute_ >= 60)
    {
        current_minute_ = 0;

        current_hour_++;

        if (current_hour_ >= 24)
        {
            current_hour_ = 0;
        }
    }
}

//manually set the clock to a specific time, if the time is not "real" it willl be converted into something displayable
void time_service_set(uint8_t hour,
                      uint8_t minute)
{
    current_hour_ = hour % 24;
    current_minute_ = minute % 60;
}

//returns current time in hours and minutes
void time_service_get(uint8_t *hour,
                      uint8_t *minute)
{
    if (hour != NULL)
    {
        *hour = current_hour_;
    }

    if (minute != NULL)
    {
        *minute = current_minute_;
    }
}

//increments hour and handles number base conversion
void time_service_increment_hour(void)
{
    current_hour_++;

    if (current_hour_ >= 24)
    {
        current_hour_ = 0;
    }
}

//increments minute and handles number base conversion
void time_service_increment_minute(void)
{
    current_minute_++;

    if (current_minute_ >= 60)
    {
        current_minute_ = 0;
    }
}

//decrements hour and handles number base conversion
void time_service_decrement_hour(void){
    if (current_hour_ <= 0)
    {
        current_hour_ = 23;
    }
    else
    {
        current_hour_--;
    }
}

//increments minute and handles number base conversion
void time_service_decrement_minute(void){
    if (current_minute_ <= 0)
    {
        current_minute_ = 59;
    }
    else
    {
        current_minute_--;
    }
}

//checks the hour mode and increments the correct column
void time_service_increment(bool hour_mode)
{
    if (hour_mode)
    {
        time_service_increment_hour();
    }
    else
    {
        time_service_increment_minute();
    }
}

//checks the hour_mode and decrements the correct column
void time_service_decrement(bool hour_mode)
{
    if (hour_mode)
    {
        time_service_decrement_hour();
    }
    else
    {
        time_service_decrement_minute();
    }
}