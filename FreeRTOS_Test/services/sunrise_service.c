#include "sunrise_service.h"
#include "stdio.h"
#include "triac_driver.h"

#include "FreeRTOS.h"
#include "task.h"

#define SUNRISE_MAX_BRIGHTNESS     100
#define SUNRISE_STEP_SIZE          1
#define SUNRISE_UPDATE_PERIOD_MS   1000

static bool sunrise_active_;

static uint8_t current_brightness_;

static TickType_t last_update_tick_;

void sunrise_service_init(void)
{
    sunrise_active_ = false;

    current_brightness_ = 0;

    last_update_tick_ = 0;

    triac_set_brightness(0);
}

void sunrise_service_start(void)
{
    sunrise_active_ = true;

    current_brightness_ = 0;

    last_update_tick_ = xTaskGetTickCount();

    triac_set_brightness(current_brightness_);
}

void sunrise_service_stop(void)
{
    sunrise_active_ = false;

    current_brightness_ = 0;

    triac_set_brightness(0);
}

bool sunrise_service_is_active(void)
{
    return sunrise_active_;
}

uint8_t sunrise_service_get_brightness(void)
{
    return current_brightness_;
}

void sunrise_service_update(void)
{
    if (!sunrise_active_)
    {
        return;
    }

    if (current_brightness_ < SUNRISE_MAX_BRIGHTNESS)
    {
        current_brightness_ += SUNRISE_STEP_SIZE;

        triac_set_brightness(current_brightness_);
    }
    else
    {
        sunrise_active_ = false;
    }
    printf("brightness: %d\n", current_brightness_);
}