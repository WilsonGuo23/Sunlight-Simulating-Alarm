#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <time.h>
#include <stdbool.h>

void time_manager_init(void);

void clock_increment(void);

struct tm* time_manager_get_time(void);

void time_manager_set_time(time_t t);

bool time_manager_is_synced(void);

#endif