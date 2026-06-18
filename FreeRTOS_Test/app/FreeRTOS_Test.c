#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "pins.h"

#include "system_init.h"
#include "drivers/buttons_driver.h"
#include "drivers/display_driver.h"
#include "drivers/triac_driver.h"

/*void alarm_task(void *p)
{
    while (1) {
        alarm_service_task();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void display_task(void *p)
{
    while (1) {
        display_service_show_time(time_service_now());
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void input_task(void *p)
{
    system_event_t e;

    while (1) {
        if (input_service_get_event(&e)) {
            alarm_service_handle_event(e);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
*/
int main(void)
{
    system_init();

    //time_service_init();
    //display_service_init();
    //input_service_init();
    //alarm_service_init();

    //xTaskCreate(alarm_task, "alarm", 512, NULL, 2, NULL);
    //xTaskCreate(display_task, "display", 256, NULL, 1, NULL);
    //xTaskCreate(input_task, "input", 256, NULL, 3, NULL);
    
    /* TEST CODE FOR DRIVERS - DELETE LATER
    display_driver_show_time(12, 34, 56);
    triac_set_brightness(5);
    button_t button = BUTTON_NONE;

    while(1){
        //printf("Triac brightness: %d\n", triac_get_brightness());
        if (buttons_read(&button) && button == BUTTON_DEC) {
            printf("Button pressed: INC\n");
        }
        if (buttons_read(&button) && button == BUTTON_INC) {
            printf("Button pressed: DEC\n");
        }
        if (buttons_read(&button) && button == BUTTON_MODE) {
            printf("Button pressed: MODE\n");
        }
        if (buttons_read(&button) && button == BUTTON_ALARM) {
            printf("Button pressed: ALARM\n");
        }
        if (buttons_read(&button) && button == BUTTON_HOUR_MIN) {
            printf("Button pressed: HOUR_MIN\n");
        }

    }
        */
    vTaskStartScheduler();
}
