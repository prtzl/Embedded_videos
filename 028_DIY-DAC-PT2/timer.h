#ifndef TIMER_H
#define TIMER_H

#include "main.h"

/**
 * TIM5 is used as System Timer with period T = 100us/0.1ms
   PRESCALER = TIMCLK(MHz)/(10000Hz) - 1
 * **/

void delay_ms(uint32_t ms);

/** Get system timer value in ms **/
__STATIC_INLINE uint32_t timer_getms(void)
{
	return TIM5->CNT /10;
}

#endif
