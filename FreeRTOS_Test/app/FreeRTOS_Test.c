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

//TODO: make alarm task able to rest until needed, instead of checking every 10 seconds. This will reduce CPU usage and allow for more accurate alarm triggering.
//TODO: comment code to explain the functionality of each task and function, making it easier to understand and maintain.
// Updates the display every 1/2 second depending on the mode (clock, set time, set alarm, display off)
void display_task(void *p)
{
    bool flicker = false;
    while (1) {
        uint8_t hour, minute;
        ui_mode_t mode = ui_service_get_mode();
        bool hour_mode = ui_service_get_hour_mode();
        if(flicker) {
            flicker = false;
        }
        else {
            flicker = true;
        }
        switch (mode)
        {
            case UI_MODE_CLOCK:
                time_service_get(&hour, &minute);
                display_service_show_time(hour, minute, hour_mode, false);
                break;

            case UI_MODE_SET_TIME:
                time_service_get(&hour, &minute);
                display_service_show_time(hour, minute, hour_mode, flicker);
                break;

            case UI_MODE_SET_ALARM:
                alarm_service_get_alarm(&hour, &minute);
                display_service_show_time(hour, minute, hour_mode, flicker);
                break;

            case UI_MODE_DISPLAY_OFF:
                display_service_clear();
                //vTaskSuspend(NULL);
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    
}

// Checks if the current time is equal to the alarm time, and starts sunrise if so
//checks if alarm should be triggered every 10 seconds
void alarm_task(void *p)
{
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
        alarm_service_update();
    }
}

//update the internal clock every minute
void time_task(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        time_service_update();
        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}

//update sunrise brightness every second
void sunrise_task(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        sunrise_service_update();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void handle_button_press(button_t button)
{
    switch (button)
    {
        case BUTTON_INC:
            if (ui_service_get_mode() == UI_MODE_SET_TIME)
            {
                time_service_increment(ui_service_get_hour_mode());
            }
            else if (ui_service_get_mode() == UI_MODE_SET_ALARM)
            {
                alarm_service_increment(ui_service_get_hour_mode());
            }
            break;

        case BUTTON_DEC:
            if (ui_service_get_mode() == UI_MODE_SET_TIME)
            {
                time_service_decrement(ui_service_get_hour_mode());
            }
            else if (ui_service_get_mode() == UI_MODE_SET_ALARM)
            {
                alarm_service_decrement(ui_service_get_hour_mode());
            }
            break;

        case BUTTON_MODE:
            ui_service_next_mode();
            break;

        case BUTTON_SNOOZE:
            sunrise_service_stop();
            break;

        case BUTTON_HOUR_MIN:
            ui_service_toggle_hour_mode();
            break;

        default:
            break;
    }

}


void button_processing_task(void *pvParameters)
{
    input_event_t evt;

    while (1)
    {
        if (xQueueReceive(input_service_get_queue(),
                          &evt,
                          portMAX_DELAY) == pdPASS && evt.type == INPUT_EVENT_PRESS)
        {
            handle_button_press(evt.button);
        }
    }
}



int main(void)
{
    system_init();

    time_service_init();
    display_service_init();
    input_service_init();
    alarm_service_init();
    ui_service_init();

    
    xTaskCreate(time_task, "time update", 256, NULL, 1, NULL); //updates clock every minute
    xTaskCreate(input_task, "input", 256, NULL, 3, NULL); //reads buttons and generates events in queue
    xTaskCreate(alarm_task, "alarm", 256, NULL, 2, NULL); //checks if alarm should be triggered every 10 seconds
    xTaskCreate(display_task, "display", 512, NULL, 1, NULL); //updates display every 1/2 second depending on mode (clock, set time, set alarm, display off)
    xTaskCreate(button_processing_task, "button_check", 512, NULL, 2, NULL); //processes button events and prints to console which button is being pressed, held, or released, and for how long
    xTaskCreate(sunrise_task, "sunrise", 256, NULL, 2, NULL); //updates sunrise brightness every second


   
    vTaskStartScheduler();
}
