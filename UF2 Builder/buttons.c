#include "buttons.h"
#include "config.h"
#include "pico/stdlib.h"
#include "dimmer.h"

volatile bool increment_flag = false;
volatile bool decrement_flag = false;
volatile bool move_flag = false;
volatile bool snooze_flag = false;
volatile bool change_mode_flag = false;

static volatile uint64_t last_interrupt_time = 0;

static void gpio_callback(uint gpio, uint32_t events)
{
    if(gpio == ZERO_CROSS_PIN)
    {
        zero_cross_callback(gpio, events);
        return;
    }
    uint64_t now = time_us_64();

    if(now - last_interrupt_time < 100000)
        return;

    last_interrupt_time = now;

    switch(gpio)
    {
        case GPIO_WATCH_PIN:
            change_mode_flag = true;
            break;

        case GPIO_INCREMENT_PIN:
            increment_flag = true;
            break;

        case GPIO_DECREMENT_PIN:
            decrement_flag = true;
            break;

        case GPIO_MOVE_PIN:
            move_flag = true;
            break;

        case SNOOZE_PIN:
            snooze_flag = true;
            break;
    }
}

void buttons_init(void)
{
    gpio_set_irq_enabled_with_callback(
        GPIO_WATCH_PIN,
        GPIO_IRQ_EDGE_FALL,
        true,
        &gpio_callback);

    gpio_set_irq_enabled(GPIO_INCREMENT_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(GPIO_DECREMENT_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(GPIO_MOVE_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled_with_callback(SNOOZE_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_set_dir(GPIO_DECREMENT_PIN, GPIO_IN);
    gpio_set_dir(GPIO_INCREMENT_PIN, GPIO_IN);
    gpio_set_dir(GPIO_MOVE_PIN, GPIO_IN);
    gpio_set_dir(GPIO_WATCH_PIN, GPIO_IN);
    gpio_set_dir(SNOOZE_PIN, GPIO_IN);

    gpio_pull_up(GPIO_DECREMENT_PIN);
    gpio_pull_up(GPIO_INCREMENT_PIN);
    gpio_pull_up(GPIO_MOVE_PIN);
    gpio_pull_up(GPIO_WATCH_PIN);
    gpio_pull_up(SNOOZE_PIN);
}
