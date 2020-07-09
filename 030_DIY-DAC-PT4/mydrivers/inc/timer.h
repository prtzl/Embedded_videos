#ifndef TIMER_H
#define TIMER_H

#include "stm32f4xx.h"

/** Timer used **/
#define TIMER_HANDLE		TIM5

/**
 * TIM5 is used as System Timer with period T = 100us/0.1ms
 * Such period is chosen as minimal prescaler value for APB1 clk = 84MHz
   PRESCALER = TIMCLK(MHz)/(1/T) - 1
 * **/
void time_init();
uint32_t timer_getVal(void);
uint32_t timer_getms(void);
void delay_ms(uint32_t ms);


#endif
