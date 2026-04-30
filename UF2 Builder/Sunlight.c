#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include <time.h>
#include <string.h>
#include "hardware/pwm.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/sntp.h"
#include "lwip/dns.h"
#include "lwip/timeouts.h"
//WIFI
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

//GPIO PINS
static char event_str[128];
#define GPIO_WATCH_PIN 16
#define GPIO_INCREMENT_PIN 17
#define GPIO_DECREMENT_PIN 18
#define GPIO_MOVE_PIN 19
#define BIT(n)  (1u<<(n))
#define BIT_MASK BIT(0)|BIT(1)|BIT(12)|BIT(16)|BIT(17)|BIT(18)|BIT(19)
#define TRIAC_PIN 0
#define ZERO_CROSS_PIN 1

//ALARM STATES
int alarm_time_minutes = 0;
int alarm_time_hours = 0;
#define MODE_SET_CURRENT_TIME 0
#define MODE_SET_ALARM_TIME 1
#define RESTING 2
int minutes_selected=1;
int current_mode;
struct tm* clock_time;
time_t display_time;
char mystr[128];
int busy=0;
bool alarm_on=true;

//LIGHT CONTROL
volatile bool zc_flag = false;
bool end_of_cycle=false;
volatile uint delay_us = 7000; //2000-7000
volatile alarm_id_t pulse_on_alarm = -1;
volatile alarm_id_t pulse_off_alarm = -1;
volatile alarm_id_t delay_decrement_alarm = -1;

//WIFI FUNCTIONS
// Called when SNTP sync completes (optional debug)
static void sntp_set_system_time_us(u32_t sec)
{
    printf("Time synced via SNTP!\n");
}

// Start SNTP
void init_sntp(void)
{
    ip_addr_t addr;
    ipaddr_aton("216.239.35.0", &addr);

    sntp_setoperatingmode(SNTP_OPMODE_POLL);  
    sntp_setserver(0, &addr);
    sntp_init();
    printf("SNTP init done\n");
}

//LIGHT CONTROL FUNCTIONS
//used to slowly increase light level
int64_t decrement_delay(alarm_id_t id, void *user_data) {
    if (!end_of_cycle){
        delay_us-=1;
    }
    
    if (delay_us<=2000){
        end_of_cycle=true;
    }
    return 10000;
}

//tells triac to end pulse
int64_t triac_off_callback(alarm_id_t id, void *user_data) {
    gpio_put(TRIAC_PIN, 0);
    return 0;
}

//tells triac to start pulse, then sets a timer for 10 microseconds to call triac_off_callback
int64_t triac_fire_callback(alarm_id_t id, void *user_data) {
    gpio_put(TRIAC_PIN, 1);
    if (pulse_off_alarm != -1) {
        cancel_alarm(pulse_off_alarm);
    }
    pulse_off_alarm = add_alarm_in_us(10, triac_off_callback, NULL, false);
    return 0;
}

//triggers when zero cross is detected by zero cross detection unit, calls triac_fire_callback to start the pulse 
void zero_cross_callback(uint gpio, uint32_t events) {
    if (alarm_on){
        if (pulse_on_alarm != -1) {
        cancel_alarm(pulse_on_alarm);
        }
        pulse_on_alarm = add_alarm_in_us(delay_us, triac_fire_callback, NULL, false);
    }
}

//ALARM/TIME FUNCIONS
//prints current time according to internal clock
void print_alarm_time(){ 
    printf("%d%d:%d%d\n",alarm_time_hours/10, alarm_time_hours%10, alarm_time_minutes/10, alarm_time_minutes%10); 
}
//change current operation mode
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

//toggles cursor between hours and minutes
void move_column(){
    if (minutes_selected){
        minutes_selected=0;
        printf("changing to hours\n");
    } else {
        minutes_selected=1;
        printf("changing to minutes\n");
    }
    
}

//triggers when the button corresponding to one of the 4 gpio pins is pressed, lets us know the user wants to change the current time.
void gpio_callback(uint gpio, uint32_t events) {
    //sleep_ms(500); //debounce
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
    //printf("GPIO %d\n", gpio); //uncomment to see which gpio pin is pressed
}

int main()
{
    display_time = time(NULL);
    stdio_init_all(); //enables connection to serial via printf 

    //PIN INITIATION
    gpio_init_mask(BIT_MASK); //enables pins 0,1,16-19
    gpio_set_irq_enabled_with_callback(16, GPIO_IRQ_EDGE_RISE, true, &gpio_callback); //sets pin 16 to trigger on rising edge and call gpio_callback when it does
    gpio_set_irq_enabled_with_callback(17, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(18, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(19, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(ZERO_CROSS_PIN,GPIO_IRQ_EDGE_RISE,true,&zero_cross_callback);
    gpio_set_dir(ZERO_CROSS_PIN, GPIO_IN);
    gpio_set_dir(TRIAC_PIN, GPIO_OUT);

    //LIGHT LEVEL INCREASE
    delay_decrement_alarm=add_alarm_in_ms(1000, decrement_delay, NULL, false);

    //CONNECT TO WIFI
    /*printf("Starting WiFi...\n");

    if (cyw43_arch_init()) {
        printf("WiFi init failed\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();

    printf("Connecting to WiFi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID,
            WIFI_PASSWORD,
            CYW43_AUTH_WPA2_AES_PSK,
            30000)) {
        printf("WiFi connect failed\n");
        return -1;
    }

    printf("Connected!\n");

    // Start SNTP (NTP time sync)
    init_sntp();

    // Wait for time to sync (simple blocking wait)
    printf("Waiting for time sync...\n");

    time_t now = 0;
    struct tm *timeinfo;

    while (now < 1700000000) {
        cyw43_arch_poll();   // 🔥 ensures network stack runs
        sleep_ms(100);
        now = time(NULL);
        //printf("Time raw: %ld\n", now);
    }
    printf("Current time: %02d:%02d:%02d\n",
        timeinfo->tm_hour,
        timeinfo->tm_min,
        timeinfo->tm_sec);*/
    while (true) {
    }
}
