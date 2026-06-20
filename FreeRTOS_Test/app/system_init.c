#include "system_init.h"
#include "stdio.h"
#include "pico/stdlib.h"
#include "pins.h"

#include "drivers/buttons_driver.h"
#include "drivers/display_driver.h"
#include "drivers/triac_driver.h"





void system_init(void)
{
    // Standard SDK initialization
    stdio_init_all();

    // Driver initialization
    buttons_init(BUTTON_INC_PIN, BUTTON_DEC_PIN, BUTTON_MODE_PIN, BUTTON_SNOOZE_PIN, BUTTON_HOUR_MIN_PIN);
    display_driver_init();
    triac_init(TRIAC_PIN, ZERO_CROSS_PIN);

    
}