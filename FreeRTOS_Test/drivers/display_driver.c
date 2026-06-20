#include <stdint.h>
#include <stdbool.h>

#include <stdint.h>
#include <stdbool.h>

#include "display_driver.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "../app/pins.h"

static uint8_t _clk_pin = DISPLAY_CLK_PIN;
static uint8_t _dio_pin = DISPLAY_DIO_PIN;

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

static void start_display(void)
{
    gpio_put(_clk_pin, 1);
    gpio_put(_dio_pin, 1);
    sleep_us(2);
    gpio_put(_dio_pin, 0);
}

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

static void write_byte(uint8_t data)
{
    for (int i = 0; i < 8; ++i)
    {
        gpio_put(_clk_pin, 0);
        gpio_put(_dio_pin, data & 0x01);
        sleep_us(3);
        data >>= 1;
        gpio_put(_clk_pin, 1);
        sleep_us(3);
    }
}

static void ack_cycle(void)
{
    uint64_t timeout = time_us_64();

    gpio_put(_clk_pin, 0);
    gpio_set_dir(_dio_pin, GPIO_IN);
    sleep_us(5);

    while (gpio_get(_dio_pin))
    {
        if (time_us_64() - timeout > 1000)
        {
            break;
        }
    }

    gpio_set_dir(_dio_pin, GPIO_OUT);
    gpio_put(_clk_pin, 1);
    sleep_us(2);
    gpio_put(_clk_pin, 0);
}

static void send_command(uint8_t command)
{
    start_display();
    write_byte(command);
    ack_cycle();
    stop_display();
}

static void write_data(const uint8_t *data, uint8_t count)
{
    start_display();
    write_byte(0xC0);
    ack_cycle();

    for (uint8_t i = 0; i < count; ++i)
    {
        write_byte(data[i]);
        ack_cycle();
    }

    stop_display();
}

static uint8_t encode_digit(uint8_t value, bool show_colon)
{
    uint8_t encoded = 0;

    if (value < 10)
    {
        encoded = digit_map[value];
    }

    if (show_colon)
    {
        encoded |= 0x80;
    }

    return encoded;
}

void display_driver_init(void)
{
    gpio_init(_clk_pin);
    gpio_init(_dio_pin);
    gpio_set_dir(_clk_pin, GPIO_OUT);
    gpio_set_dir(_dio_pin, GPIO_OUT);

    send_command(0x40);
    send_command(0x8F);
}

void display_driver_show_time(uint8_t hours, uint8_t minutes)
{
    uint8_t digits[4];
    digits[0] = encode_digit(hours / 10, false);
    digits[1] = encode_digit(hours % 10, true);
    digits[2] = encode_digit(minutes / 10, false);
    digits[3] = encode_digit(minutes % 10, false);

    write_data(digits, 4);
}