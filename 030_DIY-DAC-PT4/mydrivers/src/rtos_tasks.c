#include "rtos_tasks.h"
#include "stm32f4xx_ll_gpio.h"
#include "gui.h"
#include "rtos.h"

/**
  TASK INITS
**/
// Create LED blink task (c++ way ^^)
rtos_task_t task_blink =
{
  .name = "blink", // name task
  .task_function = blink, // pass function pointer
  .last_tick = 0,
	.period_ms = 500,
	.priority = 10
};

// Action led blink task
rtos_task_t task_blink_action =
{
  .name = "a_blink", // name task
  .task_function = blink_action, // pass function pointer
  .last_tick = 0,
	.period_ms = 100,
	.priority = 10
};

// Time counter task
rtos_task_t task_time =
{
  .name = "time", // name task
  .task_function = time_driver, // pass function pointer
  .last_tick = 0,
	.period_ms = 1000,
	.priority = -2
};
/**************************************************/

/**
 * BLINK task function
 */
void blink(void)
{  
  if( (LL_GPIO_ReadInputPort(GPIOD) & LL_GPIO_PIN_15) == 0 )
  {
    // it was turned off
    led_write(GPIOD, LL_GPIO_PIN_15, 1);
  }
  else
  {
    // it was turned on
    led_write(GPIOD, LL_GPIO_PIN_15, 0);
  }
}

/**
 * Blink led when gui action was performed
 */
uint8_t a_led_flag;
void blink_action(void)
{
	if(a_led_flag == 1)
	{
		a_led_flag = 0;
		led_write(GPIOD, LL_GPIO_PIN_14, 1);
	}
	else
	{
		led_write(GPIOD, LL_GPIO_PIN_14, 0);
	}
}

/**
 * TIME keeping function
 * It can be activated or deactivated with time_disabled parameter
 */
void time_driver(void)
{
#ifdef USE_RTOS_TIMER
	static uint32_t tajm = 0;
	tajm++;
//	screen_clock(tajm);
	display_status(tajm);
#else
//	screen_clock(0);
	display_status(0);
#endif
}

/**************************************************/

// Led write
void led_write(GPIO_TypeDef *GPIO, uint32_t pin, uint8_t state)
{
	if( state == 1 )
	{
		LL_GPIO_SetOutputPin(GPIO, pin);
	}
	else
	{
		LL_GPIO_ResetOutputPin(GPIO, pin);
	}
}

/**************************************************/

// Create list for all tasks
rtos_task_t *rtos_task_list[] = { &task_blink, &task_time, &task_blink_action, 0};
