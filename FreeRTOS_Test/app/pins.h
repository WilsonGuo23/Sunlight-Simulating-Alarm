#ifndef PINS_H
#define PINS_H

// Default pin mapping for the Pico-based sunlight-simulating alarm.
// Adjust these values to match your hardware wiring before running tests.

#define BUTTON_INC_PIN     17
#define BUTTON_DEC_PIN     18
#define BUTTON_MODE_PIN    20
#define BUTTON_SNOOZE_PIN   19
#define BUTTON_HOUR_MIN_PIN  16

#define BUTTON_ALARM_PIN BUTTON_SNOOZE_PIN

#define DISPLAY_DIO_PIN    2
#define DISPLAY_CLK_PIN    3

#define TRIAC_PIN          0
#define ZERO_CROSS_PIN     1

#define STATUS_LED_PIN     25

#endif // PINS_H
