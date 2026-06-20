#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "pins.h"

#include "system_init.h"
#include "services/time_service.h"
#include "services/display_service.h"
#include "services/input_service.h"
#include "services/alarm_service.h"
#include "services/ui_service.h"


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

    time_service_init();
    display_service_init();
    input_service_init();
    alarm_service_init();
    ui_service_init();

    xTaskCreate(time_task, "time", 256, NULL, 1, NULL);
    xTaskCreate(input_task, "input", 256, NULL, 3, NULL);
    xTaskCreate(button_debug_task, "button_debug", 512, NULL, 2, NULL);
    //
    //input_service_init();
    //alarm_service_init();

    //xTaskCreate(alarm_task, "alarm", 512, NULL, 2, NULL);
    //xTaskCreate(display_task, "display", 256, NULL, 1, NULL);
    //xTaskCreate(input_task, "input", 256, NULL, 3, NULL);

   
    vTaskStartScheduler();
}
