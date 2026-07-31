#include <stdint.h>
#include <stdbool.h

#include "stdio.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

#include "display_driver.h"

//TODO: make this non blocking

/* =========================
 * TM1637 GPIO configuration
 * ========================= */
static uint8_t _clk_pin;
static uint8_t _dio_pin;
static bool queue_processing_active = false;

typedef struct
{
    uint8_t pin;
    bool level;
    uint64_t delay;
} pin_change_t;

static QueueHandle_t pin_state_queue_;

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




static void queue_pin_state(uint8_t pin, bool level, uint64_t delay)
{
    pin_change_t change = {.pin = pin, .level = level, .delay = delay};

    if (pin_state_queue_ != NULL)
    {
        xQueueSendToBack(pin_state_queue_, &change, 0);
    }
}

static int64_t set_pin(alarm_id_t id, void *user_data)

    queue_processing_active = true;{
    pin_change_t next_change;

    if (xQueueReceiveFromISR(pin_state_queue_, &next_change, NULL) == pdPASS)
    {
        gpio_put(next_change.pin, next_change.level);
        printf("pin: %d, level: %d, delay: %llu\n", next_change.pin, next_change.level, next_change.delay);
    }
    else
    {
        queue_processing_active = false;
        return 0; // No more changes to process
    }

    if (uxQueueMessagesWaitingFromISR(pin_state_queue_) > 0)
    {
        add_alarm_in_us(next_change.delay, set_pin, NULL, true);
    }

    return 0;
}

void process_queue(){
    if (!queue_processing_active)
    
        add_alarm_in_us(0, set_pin, NULL, false);
    }
    while (queue_processing_active);
}
/* =========================================================
 * Start condition for TM1637 communication
 * CLK high → DIO high → DIO low (data start condition)
 * ========================================================= */
/*static void start_display(void)
{
    queue_pin_state(_clk_pin, 1,0);
    queue_pin_state(_dio_pin, 1,2);
    queue_pin_state(_dio_pin, 0,2);
    process_queue();
}*/
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
/*static void stop_display(void)
{
    queue_pin_state(_clk_pin,0,2);
    queue_pin_state(_dio_pin,0,2);
    queue_pin_state(_clk_pin,1,2);
    queue_pin_state(_dio_pin,1,2);
    process_queue();
}*/
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
/*static void write_byte(uint8_t data)
{
    for (int i = 0; i < 8; ++i)
    {
        queue_pin_state(_clk_pin, 0,0);
        queue_pin_state(_dio_pin, data & 0x01,3); // send LSB

        data >>= 1; // shift to next bit

        queue_pin_state(_clk_pin, 1,3);
    }
    process_queue();
}*/
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
/* static void ack_cycle(void)
{
    uint64_t timeout = time_us_64();
    gpio_put(_clk_pin, 0);

     //Release DIO line for ACK from display 
    gpio_set_dir(_dio_pin, GPIO_IN);

    // Wait for device to pull line low (ACK) 
    while (gpio_get(_dio_pin))
    {
        if (time_us_64() - timeout > 1000)
        {
            break; // timeout safety
        }
    }

    //Restore DIO as output 
    gpio_set_dir(_dio_pin, GPIO_OUT);
    queue_pin_state(_clk_pin, 1, 2);
    queue_pin_state(_clk_pin, 0, 2);
    process_queue();
}*/

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

static void big_send_command(uint8_t command)
{
    //start
    queue_pin_state(_clk_pin, 1,0);
    queue_pin_state(_dio_pin, 1,2);
    queue_pin_state(_dio_pin, 0,2);

    // write byte
    for (int i = 0; i < 8; ++i)
    {
        queue_pin_state(_clk_pin, 0,0);
        queue_pin_state(_dio_pin, command & 0x01,3); // send LSB

        command >>= 1; // shift to next bit

        queue_pin_state(_clk_pin, 1,3);
    }
    //ack
    uint64_t timeout = time_us_64();
    queue_pin_state(_clk_pin, 0, 0);
    process_queue();

     //Release DIO line for ACK from display 
    gpio_set_dir(_dio_pin, GPIO_IN);

    // Wait for device to pull line low (ACK) 
    while (gpio_get(_dio_pin))
    {
        if (time_us_64() - timeout > 1000)
        {
            printf("connection timed out\n");
            break; // timeout safety
        }
    }

    //Restore DIO as output 
    gpio_set_dir(_dio_pin, GPIO_OUT);
    queue_pin_state(_clk_pin, 1, 2);
    queue_pin_state(_clk_pin, 0, 2);
    //stop
     queue_pin_state(_clk_pin,0,2);
    queue_pin_state(_dio_pin,0,2);
    queue_pin_state(_clk_pin,1,2);
    queue_pin_state(_dio_pin,1,2);
    process_queue();

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
void display_driver_init(uint8_t clk_pin, uint8_t dio_pin)
{
    _clk_pin = clk_pin;
    _dio_pin = dio_pin;
    pin_state_queue_ = xQueueCreate1200, sizeof(pin_change_t));

    gpio_init(_clk_pin);
    gpio_init(_dio_pin);

    gpio_set_dir(_clk_pin, GPIO_OUT);
    gpio_set_dir(_dio_pin, GPIO_OUT);

    /* Set data mode + auto-increment */
   bsend_command(0x40);

    /* Set brightness (0x88–0x8F, max brightness here) */    sbnd_command(0x8F);;
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
}gits[2] = 0;
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
}its[2] = 0;
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