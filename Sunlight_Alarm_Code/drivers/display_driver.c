#include <stdint.h>
#include <stdbool.h>
#include "display_driver.h"

#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "../app/pins.h"
//TODO: make this non blocking

/* =========================
 * TM1637 GPIO configuration
 * ========================= */
static uint8_t _clk_pin = DISPLAY_CLK_PIN;
static uint8_t _dio_pin = DISPLAY_DIO_PIN;

/* =========================
 * 7-segment encoding table
 * Each byte maps a digit (0–9)
 * to TM1637 segment bit pattern
 * ========================= */
static const uint8_t digit_map[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

/* =========================================================
 * Start condition for TM1637 communication
 * CLK high → DIO high → DIO low (data start condition)
 * ========================================================= */
static void start_display(void)
{
    gpio_put(_clk_pin, 1);
    gpio_put(_dio_pin, 1);
    sleep_us(2);
    gpio_put(_dio_pin, 0);
}

/* =========================================================
 * Stop condition for TM1637 communication
 * CLK low → DIO low → CLK high → DIO high
 * ========================================================= */
static void stop_display(void)
{
    gpio_put(_clk_pin, 0);
    sleep_us(2);
    gpio_put(_dio_pin, 0);
    sleep_us(2);
    gpio_put(_clk_pin, 1);
    sleep_us(2);
    gpio_put(_dio_pin, 1);
}

/* =========================================================
 * Write one byte to TM1637 (LSB first)
 * Clock pulses shift data into display driver
 * ========================================================= */
static void write_byte(uint8_t data)
{
    for (int i = 0; i < 8; ++i)
    {
        gpio_put(_clk_pin, 0);
        gpio_put(_dio_pin, data & 0x01); // send LSB
        sleep_us(3);

        data >>= 1; // shift to next bit

        gpio_put(_clk_pin, 1);
        sleep_us(3);
    }
}

/* =========================================================
 * ACK cycle from TM1637
 * Temporarily release DIO and wait for acknowledgment
 * Includes timeout to avoid hanging
 * ========================================================= */
static void ack_cycle(void)
{
    uint64_t timeout = time_us_64();

    gpio_put(_clk_pin, 0);

    /* Release DIO line for ACK from display */
    gpio_set_dir(_dio_pin, GPIO_IN);
    sleep_us(5);

    /* Wait for device to pull line low (ACK) */
    while (gpio_get(_dio_pin))
    {
        if (time_us_64() - timeout > 1000)
        {
            break; // timeout safety
        }
    }

    /* Restore DIO as output */
    gpio_set_dir(_dio_pin, GPIO_OUT);

    gpio_put(_clk_pin, 1);
    sleep_us(2);
    gpio_put(_clk_pin, 0);
}

/* =========================================================
 * Send a command byte to TM1637
 * Used for configuration (brightness, mode, etc.)
 * ========================================================= */
static void send_command(uint8_t command)
{
    start_display();
    write_byte(command);
    ack_cycle();
    stop_display();
}

/* =========================================================
 * Write multiple bytes to display RAM
 * Used to update all 4 digits at once
 * ========================================================= */
static void write_data(const uint8_t *data, uint8_t count)
{
    start_display();

    /* Set auto-increment address mode */
    write_byte(0xC0);
    ack_cycle();

    /* Write digit data */
    for (uint8_t i = 0; i < count; ++i)
    {
        write_byte(data[i]);
        ack_cycle();
    }

    stop_display();
}

/* =========================================================
 * Convert numeric digit (0–9) into 7-segment encoding
 * Optionally adds decimal/colon bit
 * ========================================================= */
static uint8_t encode_digit(uint8_t value, bool show_colon)
{
    uint8_t encoded = 0;

    if (value < 10)
    {
        encoded = digit_map[value];
    }

    /* TM1637 colon/DP bit */
    if (show_colon)
    {
        encoded |= 0x80;
    }

    return encoded;
}

/* =========================================================
 * Initialize display GPIO and set default brightness
 * ========================================================= */
void display_driver_init(void)
{
    gpio_init(_clk_pin);
    gpio_init(_dio_pin);

    gpio_set_dir(_clk_pin, GPIO_OUT);
    gpio_set_dir(_dio_pin, GPIO_OUT);

    /* Set data mode + auto-increment */
    send_command(0x40);

    /* Set brightness (0x88–0x8F, max brightness here) */
    send_command(0x8F);
}

/* =========================================================
 * Display HH:MM time on 4-digit TM1637 display
 *
 * hour_mode:
 *   - 1 → editing hours (blink hours)
 *   - 0 → editing minutes (blink minutes)
 *
 * flicker:
 *   - used for UI blinking effect during edit mode
 * ========================================================= */
void display_driver_show_time(uint8_t hours, uint8_t minutes, bool hour_mode, bool flicker)
{
    uint8_t digits[4];

    /* Convert time into 7-segment digits */
    digits[0] = encode_digit(hours / 10, false);
    digits[1] = encode_digit(hours % 10, true);   // colon enabled here
    digits[2] = encode_digit(minutes / 10, false);
    digits[3] = encode_digit(minutes % 10, false);

    /* UI flicker logic for editing mode */
    if (flicker && hour_mode == 1)
    {
        /* Blink hours */
        digits[0] = 0;
        digits[1] = 0x80; // keep colon visible
    }
    else if (flicker && hour_mode == 0)
    {
        /* Blink minutes */
        digits[2] = 0;
        digits[3] = 0;
    }

    /* Push updated buffer to display */
    write_data(digits, 4);
}

/* =========================================================
 * Clear display (turn off all segments)
 * ========================================================= */
void display_driver_clear(void)
{
    uint8_t blank[4] = {0, 0, 0, 0};
    write_data(blank, 4);
}