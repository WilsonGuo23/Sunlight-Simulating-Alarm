#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "pins.h"
#include "drivers/buttons_driver.h"
#include "drivers/display_driver.h"
#include "drivers/triac_driver.h"

static const char *button_name(button_t button)
{
    switch (button)
    {
    case BUTTON_INC:
        return "INC";
    case BUTTON_DEC:
        return "DEC";
    case BUTTON_MODE:
        return "MODE";
    case BUTTON_ALARM:
        return "ALARM";
    default:
        return "NONE";
    }
}

static void wait_for_button_press(button_t expected)
{
    printf("PRESS TEST: Press %s button now...\n", button_name(expected));

    uint32_t start_ms = to_ms_since_boot(get_absolute_time());
    while (true)
    {
        button_t pressed;
        if (buttons_read(&pressed))
        {
            if (pressed == expected)
            {
                printf("OK: %s button detected.\n", button_name(pressed));
                while (buttons_read(&pressed))
                {
                    sleep_ms(20);
                }
                break;
            }
            else
            {
                printf("  Detected %s, waiting for %s...\n", button_name(pressed), button_name(expected));
                while (buttons_read(&pressed))
                {
                    sleep_ms(20);
                }
            }
        }

        if (to_ms_since_boot(get_absolute_time()) - start_ms > 10000)
        {
            printf("TIMEOUT: %s button not pressed within 10 seconds.\n", button_name(expected));
            break;
        }

        sleep_ms(20);
    }
}

void driver_test_run(void)
{
    stdio_init_all();
    gpio_init(STATUS_LED_PIN);
    gpio_set_dir(STATUS_LED_PIN, GPIO_OUT);
    gpio_put(STATUS_LED_PIN, 0);

    printf("\n=== Driver Test Rig ===\n");
    printf("Display driver initialization...\n");
    display_driver_init();
    display_driver_show_time(12, 34, 56);
    printf("Display test: showing 12:34 on TM1637.\n");
    sleep_ms(3000);

    printf("Triac driver initialization...\n");
    triac_init(TRIAC_PIN, ZERO_CROSS_PIN);
    triac_set_brightness(0);
    printf("Triac brightness set to 0%%, read back %u%%.\n", triac_get_brightness());
    triac_set_brightness(50);
    printf("Triac brightness set to 50%%, read back %u%%.\n", triac_get_brightness());
    triac_set_brightness(100);
    printf("Triac brightness set to 100%%, read back %u%%.\n", triac_get_brightness());
    printf("NOTE: Zero-cross events are required for actual firing.\n");
    sleep_ms(1000);

    printf("Buttons driver initialization...\n");
    buttons_init(BUTTON_INC_PIN, BUTTON_DEC_PIN, BUTTON_MODE_PIN, BUTTON_ALARM_PIN);
    wait_for_button_press(BUTTON_INC);
    wait_for_button_press(BUTTON_DEC);
    wait_for_button_press(BUTTON_MODE);
    wait_for_button_press(BUTTON_ALARM);

    printf("Button wiring test complete.\n");
    printf("Entering live button monitor. Press any button to verify.\n");

    while (true)
    {
        button_t pressed;
        if (buttons_read(&pressed) && pressed != BUTTON_NONE)
        {
            printf("Button pressed: %s\n", button_name(pressed));
            gpio_put(STATUS_LED_PIN, 1);
            sleep_ms(150);
            gpio_put(STATUS_LED_PIN, 0);
        }
        sleep_ms(50);
    }
}
