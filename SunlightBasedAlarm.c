#include <stdio.h>
#include "pico/stdlib.h"
#include <time.h>


int main()
{
    stdio_init_all();

    while (true) {
        time_t currentTime = time(NULL);
        char* currentTimeString = ctime(&currentTime);
        struct tm *gm_time = gmtime(&currentTime);
        //printf("%ld\n", currentTime);
        printf("Tm_sec: %d\n", gm_time->tm_sec);
        printf("Tm_hour: %d\n", gm_time->tm_hour);
        printf("Tm_year: %d\n", gm_time->tm_year);
        sleep_ms(5000);
        //printf(currentTimeString);
    }
}
