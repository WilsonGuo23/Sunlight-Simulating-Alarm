#include "triac_driver.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

static uint8_t triac_pin_;
static uint8_t zero_cross_pin_;

static volatile uint8_t brightness_percent_;

static int64_t triac_off_callback(alarm_id_t id, void *user_data)
{
    gpio_put(triac_pin_, 0);
    return 0;
}

static int64_t triac_fire_callback(alarm_id_t id, void *user_data)
{
    gpio_put(triac_pin_, 1);

    add_alarm_in_us(
        100,
        triac_off_callback,
        NULL,
        false);

    return 0;
}

static void zero_cross_callback(uint gpio, uint32_t events)
{
    uint32_t delay_us =
        ((100-brightness_percent_) * 6000) / 100;

    add_alarm_in_us(
        delay_us,
        triac_fire_callback,
        NULL,
        false);
}

void triac_init(uint8_t triac_pin,
                uint8_t zero_cross_pin)
{
    triac_pin_ = triac_pin;
    zero_cross_pin_ = zero_cross_pin;

    gpio_init(triac_pin_);
    gpio_set_dir(triac_pin_, GPIO_OUT);

    gpio_init(zero_cross_pin_);
    gpio_set_dir(zero_cross_pin_, GPIO_IN);

    gpio_set_irq_enabled_with_callback(
        zero_cross_pin_,
        GPIO_IRQ_EDGE_RISE,
        true,
        zero_cross_callback);

    brightness_percent_ = 0;
}

void triac_set_brightness(uint8_t percent)
{
    if (percent > 100)
        percent = 100;

    brightness_percent_ = percent;
}

uint8_t triac_get_brightness(void)
{
    return brightness_percent_;
}