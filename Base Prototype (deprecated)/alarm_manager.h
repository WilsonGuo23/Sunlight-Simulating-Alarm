#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H

#include <stdbool.h>
#include <time.h>

#define MODE_SET_CURRENT_TIME 0
#define MODE_SET_ALARM_TIME   1
#define RESTING               2

extern int alarm_time_minutes;
extern int alarm_time_hours;
extern int minutes_selected;
extern int current_mode;
extern bool alarm_on;

void alarm_manager_init(struct tm* clock_time_ptr);
void change_mode(void);
void increment_column(void);
void decrement_column(void);
void move_column(void);
void alarm_check(void);

#endif
