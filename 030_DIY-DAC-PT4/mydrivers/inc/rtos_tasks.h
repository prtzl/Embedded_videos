#ifndef RTOS_TASKS_H
#define RTOS_TASKS_H

//#include "main.h"
#include "stm32f4xx.h"

#define LED_L_PORT PORTD
#define LED_L_PIN  LL_GPIO_PIN_14
#define LED_TX_PORT PORTD
#define LED_TX_PIN  LL_GPIO_PIN_13

/** 
    Create new data type that is
    pointer to a function that 
    does not return nor accept anything
**/
typedef void(*ptr_function)(void);

// LED typedef
typedef enum
{
  led_l,
  led_tx
}led_t;

// New data type for holding tasks
typedef struct 
{
  char name[10]; // task name
  ptr_function task_function; // task function pointer
  uint32_t last_tick;  // last rtos_tick this task has been called
  uint32_t period_ms; // task period
  int8_t priority;
}rtos_task_t;

/***** PERIPHERAL FUNCTIONS *****/

// LED
void led_write(GPIO_TypeDef *GPIO, uint32_t pin, uint8_t state);
void blink(void);
void blink_action(void);

// Time
void time_driver(void);




#endif
