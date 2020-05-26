#include "timer.h"
#include "stm32f4xx_ll_tim.h"

/**
* @brief  Init my (non interrupt based) timer for delays, timeouts
* @info		A 32 bit timer used to count in 1ms period.
* 				Its DR should be used as "global timer value"
**/
void time_init()
{
  LL_TIM_SetCounter(TIMER_HANDLE, 0);
  LL_TIM_EnableCounter(TIMER_HANDLE);
}


/** Get tim value **/
uint32_t timer_getVal(void)
{
	return LL_TIM_GetCounter(TIMER_HANDLE);
}


/** Get tim value in ms **/
uint32_t timer_getms(void)
{
	return timer_getVal() /10;
}


/** Delay in ms **/
void delay_ms(uint32_t ms)
{
	uint32_t cnt = timer_getms();
	while( ( cnt + ms ) >= timer_getms() );
}
