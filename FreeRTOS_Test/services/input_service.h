#ifndef INPUT_SERVICE_H
#define INPUT_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "buttons_driver.h"

typedef enum
{
    INPUT_EVENT_PRESS = 0,
    INPUT_EVENT_HOLD,
    INPUT_EVENT_RELEASE
} input_event_type_t;

typedef struct
{
    button_t button;
    input_event_type_t type;
} input_event_t;

/*
 * Initialize queue + internal state
 */
void input_service_init(void);

/*
 * FreeRTOS task that reads buttons and generates events
 */
void input_task(void *pvParameters);

/*
 * Get queue handle (consumed by other services/tasks)
 */
QueueHandle_t input_service_get_queue(void);

/*
 * Debug task to print button events to console
 */
void button_debug_task(void *pvParameters);

#endif