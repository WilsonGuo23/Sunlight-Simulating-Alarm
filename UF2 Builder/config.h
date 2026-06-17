#ifndef CONFIG_H
#define CONFIG_H

#define GPIO_WATCH_PIN      16
#define GPIO_INCREMENT_PIN  18
#define GPIO_DECREMENT_PIN  19
#define GPIO_MOVE_PIN       17
#define SNOOZE_PIN          20

#define TRIAC_PIN           0
#define ZERO_CROSS_PIN      1

#define DIO                 2
#define CLK                 3

#define BIT(n)  (1u<<(n))
#define BIT_MASK BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(12)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)
#endif