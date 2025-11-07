#include <stdio.h>
#include "pico/stdlib.h"
#include <time.h>

static char event_str[128];
#define GPIO_WATCH_PIN 16
void gpio_event_string(char *buf, uint32_t events);

void gpio_callback(uint gpio) {
    printf("Button was pressed indicating the user wants to set a time\n");
}



int main()
{
    stdio_init_all(); //enables connection to serial via printf 
    gpio_init(GPIO_WATCH_PIN); //enables pin 16
    gpio_set_irq_enabled_with_callback(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback); //sets pin 16 to trigger on rising edge and call gpio_callback when it does
    while (true) {
    }
}
