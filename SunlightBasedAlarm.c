#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include <time.h>
static char event_str[128];
#define GPIO_WATCH_PIN 16
#define GPIO_INCREMENT_PIN 17
#define GPIO_DECREMENT_PIN 18
#define GPIO_MOVE_PIN 19
#define BIT(n)  (1u<<(n))
#define BIT_MASK BIT(16)|BIT(17)|BIT(18)|BIT(19)
//void gpio_event_string(char *buf, uint32_t events);
/*int current_time_minutes = 0;
int current_time_hours = 0;*/
int alarm_time_minutes = 0;
int alarm_time_hours = 0;
#define MODE_SET_CURRENT_TIME 0
#define MODE_SET_ALARM_TIME 1
#define RESTING 2
int minutes_selected=1;
int current_mode;
struct tm* clock_time;
time_t display_time;
void change_mode(){ //changes modes between setting current time, setting alarm time, and resting. prints current mode to serial for testing purposes
    switch(current_mode){
        case MODE_SET_CURRENT_TIME:
            current_mode++;
            printf("Set Alarm Time\n");
            print_alarm_time();
            break;
        case MODE_SET_ALARM_TIME:
            current_mode++;
            printf("Resting\n");
            break;
        case RESTING:
            current_mode=0;
            printf("Set Clock Time\n");
            break;
    }
}
void print_alarm_time(){
    printf("%d%d:%d%d\n",alarm_time_hours/10, alarm_time_hours%10, alarm_time_minutes/10, alarm_time_minutes%10);
}
//increments the current column by 1 when the corresponding button is pressed
void increment_column(){ //TODO:finish current time setting
    if(current_mode == MODE_SET_ALARM_TIME){
        if(minutes_selected){
        alarm_time_minutes++;
        if(alarm_time_minutes > 59){
            alarm_time_minutes = 0;
        }
        } 
    else {
        alarm_time_hours++;
        if(alarm_time_hours > 23){
            alarm_time_hours = 0;
        }
    }
    print_alarm_time();
    }
}

void decrement_column(){ //TODO:finish current time setting
    if(current_mode == MODE_SET_ALARM_TIME){
        if(minutes_selected){
        alarm_time_minutes--;
        if(alarm_time_minutes < 0){
            alarm_time_minutes = 59;
        }
        } 
        else {
            alarm_time_hours--;
            if(alarm_time_hours < 0){
                alarm_time_hours = 23;
            }
        }
        print_alarm_time();
    }
}

void move_column(){ //TODO:
    if (minutes_selected){
        minutes_selected=0;
        printf("changing to hours\n");
    } else {
        minutes_selected=1;
        printf("changing to minutes\n");
    }
    
}

//triggers when the button corresponding to GP16 is pressed, lets us know the user wants to change the current time.
void gpio_callback(uint gpio, uint32_t events) {
    sleep_ms(500); //debounce
    switch(gpio){
        case GPIO_WATCH_PIN:
            change_mode();
            break;
        case GPIO_INCREMENT_PIN:
            increment_column();
            break;
        case GPIO_DECREMENT_PIN:
            decrement_column();
            break;
        case GPIO_MOVE_PIN:
            move_column();
            break;
    }
    //printf("GPIO %d\n", gpio); uncomment to see which gpio pin is pressed
}

int main()
{
    display_time = time(NULL);
    stdio_init_all(); //enables connection to serial via printf 
    gpio_init_mask(BIT_MASK); //enables pins 16-19
    gpio_set_irq_enabled_with_callback(16, GPIO_IRQ_EDGE_RISE, true, &gpio_callback); //sets pin 16 to trigger on rising edge and call gpio_callback when it does
    gpio_set_irq_enabled_with_callback(17, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(18, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(19, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    while (true) {

    }
}
