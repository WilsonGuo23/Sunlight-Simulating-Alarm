#include "input_service.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "stdio.h"

/* ================= CONFIG ================= */

#define DEBOUNCE_MS        30
#define HOLD_MS            500
#define REPEAT_MS          150

/* ================= STATE ================= */

static QueueHandle_t input_queue_;

/* Debounce/state tracking */
static button_t last_raw_ = BUTTON_NONE;
static button_t stable_ = BUTTON_NONE;

static uint32_t last_change_time_ = 0;
static uint32_t press_time_ = 0;
static uint32_t last_repeat_time_ = 0;

/* ================= INIT ================= */

void input_service_init(void)
{
    input_queue_ = xQueueCreate(10, sizeof(input_event_t));

    last_raw_ = BUTTON_NONE;
    stable_ = BUTTON_NONE;

    last_change_time_ = 0;
    press_time_ = 0;
    last_repeat_time_ = 0;
}

/* ================= ACCESSOR ================= */

QueueHandle_t input_service_get_queue(void)
{
    return input_queue_;
}

/* ================= TASK ================= */

void input_task(void *pvParameters)
{
    (void)pvParameters;

    button_t raw;
    uint32_t now;

    while (1)
    {
        now = to_ms_since_boot(get_absolute_time());

        /* ---- Read raw GPIO ---- */
        if (buttons_read(&raw))
        {
            /* debounce tracking */
            if (raw != last_raw_)
            {
                last_raw_ = raw;
                last_change_time_ = now;
            }

            /* stable press detected */
            if ((now - last_change_time_) > DEBOUNCE_MS)
            {
                if (stable_ != raw)
                {
                    stable_ = raw;

                    /* PRESS EVENT */
                    input_event_t evt =
                    {
                        .button = stable_,
                        .type = INPUT_EVENT_PRESS
                    };
                    xQueueSend(input_queue_, &evt, 0);

                    press_time_ = now;
                    last_repeat_time_ = now;
                }
            }

            /* HOLD + REPEAT */
            if (stable_ != BUTTON_NONE)
            {
                if ((now - press_time_) > HOLD_MS)
                {
                    if ((now - last_repeat_time_) > REPEAT_MS)
                    {
                        last_repeat_time_ = now;

                        input_event_t evt =
                        {
                            .button = stable_,
                            .type = INPUT_EVENT_HOLD
                        };

                        xQueueSend(input_queue_, &evt, 0);
                    }
                }
            }
        }
        else
        {
            /* ---- RELEASE detection ---- */
            if (stable_ != BUTTON_NONE)
            {
                input_event_t evt =
                {
                    .button = stable_,
                    .type = INPUT_EVENT_RELEASE
                };

                xQueueSend(input_queue_, &evt, 0);

                stable_ = BUTTON_NONE;
                last_raw_ = BUTTON_NONE;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}