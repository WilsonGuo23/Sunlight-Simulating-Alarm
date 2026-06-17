#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H
#define configENABLE_MPU                 0
#define INCLUDE_vTaskDelay 1
#define configUSE_IDLE_HOOK              0
#define configUSE_TICK_HOOK              0
#define configCHECK_FOR_STACK_OVERFLOW 0
#define configUSE_TIMERS                 1
#define configTIMER_TASK_PRIORITY        2
#define configTIMER_QUEUE_LENGTH         10
#define configTIMER_TASK_STACK_DEPTH     256
#define configNUMBER_OF_CORES 1
#define configTICK_CORE 0

#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1

#define configCPU_CLOCK_HZ                      (150000000)

#define configTICK_RATE_HZ                      (1000)

#define configMAX_PRIORITIES                    5

#define configMINIMAL_STACK_SIZE                256

#define configTOTAL_HEAP_SIZE                   (32 * 1024)

#define configMAX_TASK_NAME_LEN                 16

#define configUSE_16_BIT_TICKS                  0

#define configUSE_MUTEXES                       1

#define configUSE_COUNTING_SEMAPHORES           1

#define configUSE_RECURSIVE_MUTEXES             1


#define configSUPPORT_DYNAMIC_ALLOCATION        1

#define configSUPPORT_STATIC_ALLOCATION         0

/* Disable Pico SDK sync interop support when running FreeRTOS on RP2040.
 * This avoids the RP2040 FreeRTOS port attempting to use SDK event groups
 * and hardware spinlock wakeup paths that are not configured in this project. */
#define configSUPPORT_PICO_SYNC_INTEROP         0
#define configSUPPORT_PICO_TIME_INTEROP         0

#include <stdint.h>

extern uint32_t SystemCoreClock;

#endif