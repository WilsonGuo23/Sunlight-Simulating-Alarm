#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
static char event_str[128];
#define GPIO_WATCH_PIN 16
#define GPIO_INCREMENT_PIN 17
#define GPIO_DECREMENT_PIN 18
#define GPIO_MOVE_PIN 19
#define BIT(n)  (1u<<(n))
#define BIT_MASK BIT(16)|BIT(17)|BIT(18)|BIT(19)
//void gpio_event_string(char *buf, uint32_t events);
int current_time_minutes = 0;
int current_time_hours = 0;
int alarm_time_minutes = 0;
int alarm_time_hours = 0;

static const char *gpio_irq_str[] = {
        "LEVEL_LOW",  // 0x1
        "LEVEL_HIGH", // 0x2
        "EDGE_FALL",  // 0x4
        "EDGE_RISE"   // 0x8
};
void gpio_event_string(char *buf, uint32_t events) {
    for (uint i = 0; i < 4; i++) {
        uint mask = (1 << i);
        if (events & mask) {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0') {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events) {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}
//triggers when the button corresponding to GP16 is pressed, lets us know the user wants to change the current time.
void gpio_callback(uint gpio, uint32_t events) {
    sleep_ms(50); //debounce
    //gpio_event_string(event_str, events);
    //printf("GPIO %d %s\n", gpio, event_str);
    printf("Button was pressed indicating the user wants to set a time, enter a time in this format: HH.MM\n");
    
}
//increments the current column by 1 when the corresponding button is pressed
void increment_column(uint gpio, uint32_t events){
    printf("Incrementing current column by 1");
}

void decrement_column(uint gpio, uint32_t events){
    printf("Decrementing current column by 1");
}

void move_column(uint gpio, uint32_t events){
    printf("Moving over 1 column");
}


int main()
{
    stdio_init_all(); //enables connection to serial via printf 
    gpio_init_mask(BIT_MASK); //enables pins 16-19
    gpio_set_irq_enabled_with_callback(16, GPIO_IRQ_EDGE_RISE, true, &gpio_callback); //sets pin 16 to trigger on rising edge and call gpio_callback when it does
    gpio_set_irq_enabled_with_callback(17, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(18, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(19, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    while (true) {
    }
}
