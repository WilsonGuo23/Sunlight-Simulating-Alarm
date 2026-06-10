//TODO: seperate code into multiple files for better organization and readability, also makes it easier to navigate and understand
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include <time.h>
#include <string.h>
#include "pico/cyw43_arch.h"
#include "lwip/apps/sntp.h"
#include "lwip/dns.h"
#include "lwip/timeouts.h"
//#include "time_display.h"

//WIFI
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

//GPIO PINS
static char event_str[128];
static volatile uint64_t last_interrupt_time = 0;
static volatile uint64_t last_zero_cross_time = 0;
volatile bool increment_flag=false;
volatile bool decrement_flag=false;
volatile bool move_flag=false;
volatile bool snooze_flag=false;
volatile bool change_mode_flag=false;
#define GPIO_WATCH_PIN 16
#define GPIO_INCREMENT_PIN 17
#define GPIO_DECREMENT_PIN 18
#define GPIO_MOVE_PIN 19
#define SNOOZE_PIN 20
#define BIT(n)  (1u<<(n))
#define BIT_MASK BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(12)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)
#define TRIAC_PIN 0
#define ZERO_CROSS_PIN 1
#define DIO 2
#define CLK 3

// =========================
// DIGIT MAP
// =========================
static const uint8_t digit_map[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

//time display updates
    uint64_t last_clock_update = 0;
    uint64_t last_display_update = 0;
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
bool alarm_on=false;
volatile alarm_id_t timing_delay = -1;
bool flicker=false;

//LIGHT CONTROL
volatile bool zc_flag = false;
bool end_of_cycle=false;
volatile uint delay_us = 6000; //2000-6000
volatile alarm_id_t delay_decrement_alarm = -1;
uint64_t last_delay_decrement_update = 0;


void no_block_delay_us(int delay_us) {
    uint64_t start = time_us_64();
    while ((time_us_64() - start) < delay_us) {
    }
}
//Display Functions
static void start_display() {
    gpio_put(CLK, 1);
    gpio_put(DIO, 1);
    no_block_delay_us(2);
    gpio_put(DIO, 0);
}

static void stop_display() {
    gpio_put(CLK, 0);
    no_block_delay_us(2);
    gpio_put(DIO, 0);
    no_block_delay_us(2);
    gpio_put(CLK, 1);
    no_block_delay_us(2);
    gpio_put(DIO, 1);
}

static void write_byte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        gpio_put(CLK, 0);
        gpio_put(DIO, data & 0x01);
        no_block_delay_us(3);
        data >>= 1;
        gpio_put(CLK, 1);
        no_block_delay_us(3);
    }
}

void ask(void){    
    // ACK cycle
    uint64_t timeout = time_us_64();
    gpio_put(CLK, 0);
    gpio_set_dir(DIO, GPIO_IN);
    no_block_delay_us(5);
    while(gpio_get(DIO)){
        if(time_us_64() - timeout > 1000)
        {
        break;
    }
    }
    gpio_set_dir(DIO, GPIO_OUT);
    gpio_put(CLK, 1);
    no_block_delay_us(2);
    gpio_put(CLK, 0);
}

void display(void)
{

    // -------------------------
    // Set auto-increment mode
    // -------------------------
    start_display();
    write_byte(0x40);
    ask();
    stop_display();
    // -------------------------
    // Set display address
    // -------------------------
    start_display();
    write_byte(0xC0);
    ask();
    // -------------------------
    // Send 6 bytes of segment data
    // -------------------------
    for (int i = 0; i < 6; i++)
    {
        write_byte(0xFF);
        ask();
    }
    stop_display();
    // -------------------------
    // Display ON + max brightness
    // -------------------------
    start_display();
    write_byte(0x8F);
    ask();
    stop_display();
}

void set_display_time(int hours,int minutes) {
    uint8_t display_data[4];
    if (flicker&&current_mode!=RESTING){
        flicker=0;
        if(minutes_selected){
            display_data[0] = digit_map[hours / 10];
            display_data[1] = digit_map[hours % 10];
            display_data[2] = 0x00; // Blank
            display_data[3] = 0x00; // Blank
        }
    
        else{
            display_data[0] = 0x00; // Blank
            display_data[1] = 0x00; // Blank
            display_data[2] = digit_map[minutes / 10];
            display_data[3] = digit_map[minutes % 10];
        }
    }

    else{
        flicker=1;
        display_data[0] = digit_map[hours / 10];
        display_data[1] = digit_map[hours % 10];
        display_data[2] = digit_map[minutes / 10];
        display_data[3] = digit_map[minutes % 10];
    };

    start_display();
    write_byte(0x40);
    ask();
    stop_display();

    start_display();
    write_byte(0xC0);
    ask();

    for (int i = 0; i < 4; i++)
    {
        write_byte(display_data[i]);
        ask();
    }
    stop_display();
}

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
void decrement_delay(){
    if (!end_of_cycle){
        delay_us-=10;
    }
    if (delay_us<=2000){
        end_of_cycle=true;
    }
}

int64_t triac_fire_callback_callback_end(alarm_id_t id, void *user_data) {
    gpio_put(TRIAC_PIN, 0);
    return 0;
}

int64_t triac_fire_callback(alarm_id_t id, void *user_data) {
    gpio_put(TRIAC_PIN, 1);
    add_alarm_in_us(50,triac_fire_callback_callback_end, NULL, true);
    return 0;
}

//triggers when zero cross is detected by zero cross detection unit, sets output to high for a certain amount of time based on delay_us variable to control brightness of light, then sets output back to low until next zero cross is detected
void zero_cross_callback(uint gpio, uint32_t events)
{
    uint64_t now = time_us_64();

    if(now - last_zero_cross_time < 5000)
        return;

    last_zero_cross_time = now;

    add_alarm_in_us(delay_us, triac_fire_callback, NULL, true);
}

//ALARM/TIME FUNCIONS

//change current operation mode
void change_mode(){ //changes modes between setting current time, setting alarm time, and resting. prints current mode to serial for testing purposes
    switch(current_mode){
        case MODE_SET_CURRENT_TIME:
            current_mode++;
            //printf("Set Alarm Time\n");
            break;
        case MODE_SET_ALARM_TIME:
            current_mode++;
            //printf("Resting\n");
            break;
        case RESTING:
            current_mode=0;
            //printf("Set Clock Time\n");
            break;
    }
}

//increments the current column by 1 when the corresponding button is pressed
void increment_column(){ 
    if(current_mode == MODE_SET_ALARM_TIME){
        if(minutes_selected){
            alarm_time_minutes++;
            if(alarm_time_minutes >= 60){
                alarm_time_minutes = 0;
            }
        } 
        else {
            alarm_time_hours++;
            if(alarm_time_hours >= 24){
                alarm_time_hours = 0;
            }
        }
    }
    if (current_mode == MODE_SET_CURRENT_TIME){
        if (minutes_selected){
            clock_time->tm_min++;
            if (clock_time->tm_min >= 60){
            clock_time->tm_min = 0;
            }
        }
        else {
            clock_time->tm_hour++;
            if (clock_time->tm_hour >= 24){
                clock_time->tm_hour = 0;
            }
        }
    }
}

void decrement_column(){ 
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
    }
    if(current_mode == MODE_SET_CURRENT_TIME){
        if (minutes_selected){
            clock_time->tm_min--;
            if (clock_time->tm_min < 0){
                clock_time->tm_min = 59;
            }
        }
        else {
            clock_time->tm_hour--;
            if (clock_time->tm_hour < 0){
                clock_time->tm_hour = 23;
            }
        }
    }
}

//toggles cursor between hours and minutes
void move_column(){
    if (minutes_selected){
        minutes_selected=0;
        //printf("changing to hours\n");
    } else {
        minutes_selected=1;
        //printf("changing to minutes\n");
    }
    
}

//clock time handler
void clock_increment(){
    clock_time->tm_sec++; 
        if (clock_time->tm_sec >= 60) {
            clock_time->tm_sec = 0;
            clock_time->tm_min++;
            if (clock_time->tm_min >= 60) {
                clock_time->tm_min = 0;
                clock_time->tm_hour++;
                if (clock_time->tm_hour >= 24) {
                    clock_time->tm_hour = 0;
                }
            }
        }
}

//display handler
void display_handler(){
    if(current_mode==MODE_SET_ALARM_TIME){
            set_display_time(alarm_time_hours,alarm_time_minutes);
        }
        else{
            set_display_time(clock_time->tm_hour,clock_time->tm_min);    
        }
}

void alarm_check(){
    if (alarm_time_hours == clock_time->tm_hour && alarm_time_minutes == clock_time->tm_min) {
            alarm_on=true;
            gpio_set_irq_enabled(ZERO_CROSS_PIN,GPIO_IRQ_EDGE_FALL,true);
    }
}

//triggers when the button corresponding to one of the 4 gpio pins is pressed, lets us know the user wants to change the current time.
void gpio_callback(uint gpio, uint32_t events) {
    if (alarm_on && gpio==ZERO_CROSS_PIN) {
            zero_cross_callback(gpio, events);
            return;
    }
    uint64_t now = time_us_64();
    if(now - last_interrupt_time < 100000)
        return;
    last_interrupt_time = now;
        switch(gpio){
            case GPIO_WATCH_PIN:
                change_mode_flag=true;
                break;
            case GPIO_INCREMENT_PIN:
                increment_flag=true;
                break;
            case GPIO_DECREMENT_PIN:
                decrement_flag=true;
                break;
            case GPIO_MOVE_PIN:
                move_flag=true;
                break;
            case SNOOZE_PIN:
                snooze_flag=true;
                break;
            
        }
    
    //printf("GPIO %d\n", gpio); //uncomment to see which gpio pin is pressed
}

//Master initialization
static void master_init() {
    stdio_init_all(); //enables connection to serial via printf 

    //PIN INITIATION
    gpio_init_mask(BIT_MASK); //enables pins 0-3,16-19
    gpio_set_irq_enabled_with_callback(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback); //sets pin 16 to trigger on rising edge and call gpio_callback when it does
    gpio_set_irq_enabled(GPIO_INCREMENT_PIN,GPIO_IRQ_EDGE_FALL,true);
    gpio_set_irq_enabled(GPIO_DECREMENT_PIN,GPIO_IRQ_EDGE_FALL,true);
    gpio_set_irq_enabled(GPIO_MOVE_PIN,GPIO_IRQ_EDGE_FALL,true);
    gpio_set_irq_enabled(ZERO_CROSS_PIN,GPIO_IRQ_EDGE_FALL,true);
    gpio_set_irq_enabled(SNOOZE_PIN,GPIO_IRQ_EDGE_FALL,true);
    gpio_set_dir(ZERO_CROSS_PIN, GPIO_IN);
    gpio_set_dir(TRIAC_PIN, GPIO_OUT);
    gpio_set_dir(DIO, GPIO_OUT);
    gpio_set_dir(CLK, GPIO_OUT);
    gpio_set_dir(GPIO_DECREMENT_PIN, GPIO_IN);
    gpio_set_dir(GPIO_INCREMENT_PIN, GPIO_IN); 
    gpio_set_dir(GPIO_MOVE_PIN, GPIO_IN);
    gpio_set_dir(GPIO_WATCH_PIN, GPIO_IN);
    gpio_set_dir(SNOOZE_PIN, GPIO_IN);
    gpio_pull_up(GPIO_DECREMENT_PIN);
    gpio_pull_up(GPIO_INCREMENT_PIN);
    gpio_pull_up(GPIO_MOVE_PIN);
    gpio_pull_up(GPIO_WATCH_PIN);
    gpio_pull_up(SNOOZE_PIN);

    //Create Timers
    clock_time = localtime(&display_time);
    
}

int main()
{
    display_time = time(NULL);
    master_init();
    display();


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
        uint64_t now1 = time_us_64();
        if(change_mode_flag){
                change_mode();
                change_mode_flag=false;
        }
        if (increment_flag){
            increment_column();
            increment_flag=false;
        }
        if(decrement_flag){
            decrement_column();
            decrement_flag=false;
        }
        if(move_flag){
            move_column();
            move_flag=false;
        }
        if(snooze_flag){
            alarm_on=false;
            snooze_flag=false;
            delay_us=6000;
            gpio_set_irq_enabled(ZERO_CROSS_PIN,GPIO_IRQ_EDGE_FALL,false);
        }
        if(now1 - last_display_update >= 500000){
            last_display_update = now1;
            display_handler();
        }
        if(now1 - last_clock_update >= 1000000){
            last_clock_update = now1;
            clock_increment();
            alarm_check();
        }
        if(now1 - last_delay_decrement_update >= 10000 && alarm_on){
            last_delay_decrement_update = now1;
            decrement_delay();
        }
    }
}
