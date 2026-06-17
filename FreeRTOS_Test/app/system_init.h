#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#define BIT(n)  (1u<<(n))
#define BIT_MASK BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(12)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)

void system_init(void);
#endif