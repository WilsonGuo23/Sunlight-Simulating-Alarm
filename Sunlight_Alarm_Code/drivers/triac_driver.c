#include "triac_driver.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

/* =========================
 * GPIO configuration
 * ========================= */
static uint8_t triac_pin_;
static uint8_t zero_cross_pin_;

/* Brightness level used to calculate phase delay
 * volatile because it is accessed inside ISR context
 */
static volatile uint8_t brightness_percent_;

/* =========================================================
 * TRIAC OFF callback
 * Fired shortly after TRIAC is triggered to ensure
 * gate pulse is short (prevents continuous conduction)
 * ========================================================= */
static int64_t triac_off_callback(alarm_id_t id, void *user_data)
{
    gpio_put(triac_pin_, 0);   // turn off gate drive
    return 0;                   // do not repeat alarm
}

/* =========================================================
 * TRIAC FIRE callback
 * Activates triac gate pulse after calculated delay
 * from zero-cross event (phase control timing)
 * ========================================================= */
static int64_t triac_fire_callback(alarm_id_t id, void *user_data)
{
    gpio_put(triac_pin_, 1);   // fire triac gate

    /* Keep gate HIGH for ~100us to ensure proper latching */
    add_alarm_in_us(
        100,
        triac_off_callback,
        NULL,
        false);

    return 0;
}

/* =========================================================
 * ZERO-CROSS interrupt callback
 *
 * Triggered every AC half-cycle (100Hz on 50Hz mains / 120Hz on 60Hz)
 *
 * Converts brightness (0–100%) into phase delay:
 * - 0%   → long delay (dim)
 * - 100% → short delay (bright)
 *
 * Then schedules TRIAC firing using a hardware alarm
 * ========================================================= */
static void zero_cross_callback(uint gpio, uint32_t events)
{
    /* Phase delay calculation:
     * 7000us = approximate half-cycle window
     * 850us  = minimum safe firing delay
     */
    uint32_t delay_us =
        7000 - ((brightness_percent_ * (7000 - 850)) / 100);

    /* Schedule TRIAC firing after calculated delay */
    add_alarm_in_us(
        delay_us,
        triac_fire_callback,
        NULL,
        false);
}

/* =========================================================
 * Initialize TRIAC control system
 *
 * - Configures triac gate pin as output
 * - Configures zero-cross input interrupt
 * - Registers ISR callback for phase control timing
 * ========================================================= */
void triac_init(uint8_t triac_pin,
                uint8_t zero_cross_pin)
{
    triac_pin_ = triac_pin;
    zero_cross_pin_ = zero_cross_pin;

    /* TRIAC gate output */
    gpio_init(triac_pin_);
    gpio_set_dir(triac_pin_, GPIO_OUT);

    /* Zero-cross input */
    gpio_init(zero_cross_pin_);
    gpio_set_dir(zero_cross_pin_, GPIO_IN);

    /* Attach interrupt on AC waveform zero crossing */
    gpio_set_irq_enabled_with_callback(
        zero_cross_pin_,
        GPIO_IRQ_EDGE_RISE,
        true,
        zero_cross_callback);

    brightness_percent_ = 0;
}

/* =========================================================
 * Set brightness level (0–100%)
 *
 * Used by higher-level services (sunrise / UI / alarm)
 * ========================================================= */
void triac_set_brightness(uint8_t percent)
{
    if (percent > 100)
        percent = 100;

    brightness_percent_ = percent;
}

/* =========================================================
 * Get current brightness level
 * ========================================================= */
uint8_t triac_get_brightness(void)
{
    return brightness_percent_;
}