#include "time_service.h"

#include "FreeRTOS.h"
#include "task.h"

static uint8_t current_hour_;
static uint8_t current_minute_;

static TickType_t last_tick_;

void time_service_init(void)
{
    current_hour_ = 0;
    current_minute_ = 0;

    last_tick_ = xTaskGetTickCount();
}

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


void time_service_set(uint8_t hour,
                      uint8_t minute)
{
    current_hour_ = hour % 24;
    current_minute_ = minute % 60;
}

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

void time_service_increment_hour(void)
{
    current_hour_++;

    if (current_hour_ >= 24)
    {
        current_hour_ = 0;
    }
}

void time_service_increment_minute(void)
{
    current_minute_++;

    if (current_minute_ >= 60)
    {

        current_minute_ = 0;
    }
}

void time_task(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(60000));
        uint8_t hour, minute;
        time_service_update();
        time_service_get(&hour, &minute);
        //display_service_show_time(hour, minute);
    }
}