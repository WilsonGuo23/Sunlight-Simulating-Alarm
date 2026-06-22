#include "buttons_driver.h"
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "stdio.h"

static uint8_t inc_pin_;
static uint8_t dec_pin_;
static uint8_t mode_pin_;
static uint8_t snooze_pin_;
static uint8_t hour_min_pin_;

//initial setup 
void buttons_init(uint8_t inc_pin,
                  uint8_t dec_pin,
                  uint8_t mode_pin,
                  uint8_t snooze_pin,
                  uint8_t hour_min_pin)
{
    //set pins from pins.h
    inc_pin_ = inc_pin;
    dec_pin_ = dec_pin;
    mode_pin_ = mode_pin;
    snooze_pin_ = snooze_pin;
    hour_min_pin_ = hour_min_pin;

    //enables pins in register
    gpio_init(inc_pin_);
    gpio_init(dec_pin_);
    gpio_init(mode_pin_);
    gpio_init(snooze_pin_);
    gpio_init(hour_min_pin_);

    //set to active low
    gpio_pull_up(inc_pin_);
    gpio_pull_up(dec_pin_);
    gpio_pull_up(mode_pin_);
    gpio_pull_up(snooze_pin_);
    gpio_pull_up(hour_min_pin_);
}

//checks all initialized pins for a button press. Buttons are "pressed" when set to low
bool buttons_read(button_t *button)
{
    if (!gpio_get(inc_pin_))
    {
        *button = BUTTON_INC;
        return true;
    }

    if (!gpio_get(dec_pin_))
    {
        *button = BUTTON_DEC;
        return true;
    }

    if (!gpio_get(mode_pin_))
    {
        *button = BUTTON_MODE;
        return true;
    }

    if (!gpio_get(snooze_pin_))
    {
        *button = BUTTON_SNOOZE;
        return true;
    }

    if (!gpio_get(hour_min_pin_))
    {
        *button = BUTTON_HOUR_MIN;
        return true;
    }

    return false;
}

//converts button_t type to a string for debugging purposes since enums do not automatically have this function
static void button_to_string(button_t *button)
{
    switch (*button)
    {
        case BUTTON_INC:
            printf("INCREMENT");
            break;

        case BUTTON_DEC:
            printf("DECREMENT");
            break;

        case BUTTON_MODE:
            printf("MODE");
            break;

        case BUTTON_SNOOZE:
            printf("SNOOZE");
            break;

        case BUTTON_HOUR_MIN:
            printf("HOUR_MIN");
            break;

        default:
            printf("UNKNOWN");
            break;
    }
}