#include "display.h"

#include "pico/stdlib.h"
#include "config.h"

static const uint8_t digit_map[10] =
{
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static void no_block_delay_us(int delay_us)
{
    uint64_t start = time_us_64();

    while ((time_us_64() - start) < delay_us)
    {
    }
}

static void start_display(void)
{
    gpio_put(CLK, 1);
    gpio_put(DIO, 1);

    no_block_delay_us(2);

    gpio_put(DIO, 0);
}

static void stop_display(void)
{
    gpio_put(CLK, 0);

    no_block_delay_us(2);

    gpio_put(DIO, 0);

    no_block_delay_us(2);

    gpio_put(CLK, 1);

    no_block_delay_us(2);

    gpio_put(DIO, 1);
}

static void write_byte(uint8_t data)
{
    for(int i = 0; i < 8; i++)
    {
        gpio_put(CLK, 0);

        gpio_put(DIO, data & 0x01);

        no_block_delay_us(3);

        data >>= 1;

        gpio_put(CLK, 1);

        no_block_delay_us(3);
    }
}

static void ack_cycle(void)
{
    uint64_t timeout = time_us_64();

    gpio_put(CLK, 0);

    gpio_set_dir(DIO, GPIO_IN);

    no_block_delay_us(5);

    while(gpio_get(DIO))
    {
        if(time_us_64() - timeout > 1000)
        {
            break;
        }
    }

    gpio_set_dir(DIO, GPIO_OUT);

    gpio_put(CLK, 1);

    no_block_delay_us(2);

    gpio_put(CLK, 0);
}

void display_init(void)
{
    gpio_set_dir(DIO, GPIO_OUT);
    gpio_set_dir(CLK, GPIO_OUT);

    start_display();
    write_byte(0x40);
    ack_cycle();
    stop_display();

    start_display();
    write_byte(0x8F);
    ack_cycle();
    stop_display();
}

void display_show_time(
    int hours,
    int minutes,
    int blink_hours,
    int blink_minutes)
{
    uint8_t data[4];

    if(blink_hours)
    {
        data[0] = 0x00;
        data[1] = 0x00;
    }
    else
    {
        data[0] = digit_map[hours / 10];
        data[1] = digit_map[hours % 10];
    }

    if(blink_minutes)
    {
        data[2] = 0x00;
        data[3] = 0x00;
    }
    else
    {
        data[2] = digit_map[minutes / 10];
        data[3] = digit_map[minutes % 10];
    }

    start_display();

    write_byte(0x40);

    ack_cycle();

    stop_display();

    start_display();

    write_byte(0xC0);

    ack_cycle();

    for(int i = 0; i < 4; i++)
    {
        write_byte(data[i]);
        ack_cycle();
    }

    stop_display();
}