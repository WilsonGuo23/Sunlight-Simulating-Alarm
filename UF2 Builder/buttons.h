#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h>
#include <stdint.h>

extern volatile bool increment_flag;
extern volatile bool decrement_flag;
extern volatile bool move_flag;
extern volatile bool snooze_flag;
extern volatile bool change_mode_flag;

void buttons_init(void);

#endif
