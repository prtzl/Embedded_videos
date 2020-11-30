/* Frequency & period counter library for STM32, HAL and LL
 *
 * This library will use either HAL or LL function depending
 * on your CubeMx settings. This is detected by library so no
 * extra work has to be done by you. Library can use ISR or DMA
 * to copy timer register data. See following instructions.
 *
 * Example code tested on STM32F407VG is provided.
 *
 * All configurations can be done in fpCounter_Config.h where you
 * input handles and macros and pwm output channel for your timers
 * for measurement and for generating demo PWM signal.
 *
 * To configure demo PWM signal timer, do the following:
 * 	- Set internal clock
 * 	- Set PWM1 on output channel (1 or 2 or 3 or 4)
 *	- Set timer prescaler so that timer frequency will be 1MHz
 *	  fin = 40MHz -> division by 40 -> prescaler = 39
 *	- Set ARR to 999 for counter to overrun at 1kHz
 *	- Set compare register to 1/2 of ARR to set duty cycle to 50%
 *
 * To configure period measurement timer, do the following (integrated way):
 *  - Set "Combined channels" to PWM input on CH1
 *   -> this will give you one input pin and both input channels
 *   -> will be configured to take input signal from that one.
 *   -> Also timer will be configured to be in slave reset mode,
 *   -> triggered by TI1FP1 signal. Also you can see that CH1 is
 *   -> triggered on rising edge and CH2 is triggered on falling edge
 *  - Enable either interrupts of DMA for CCRx data copying:
 *	 - Interrupts:
 *	  -> Enable interrupts for this timer by ticking the box in NVIC settings.
 *    -> Go to Cube NVIC settings to confirm that timer interrupt is enabled
 *       give is priority of 1 or lower (larger number)
 *	 - DMA:
 *	  -> Enable DMA requests for both channels in DMA settings box
 *	  -> Click Add and add requests for TIMx_CH1 and TIMx_CH2
 *	  -> Configure TIMx_CH1 mode as circular, un-tick any Increment address
 *	  	 option for peripheral and memory
 *	  -> Set up peripheral and Memory Data Width as Half Word
 *	  -> Remember which DMAx and STREAMy is used and fill that info
 *	     in "fpCounter_Config.h" if you are using LL drivers
 *
 * How to setup library:
 *  - Include this library in your main.c.
 *	- If you want to use interrupts for copying data:
 *	 -> Include library in stm32abxx_it.c
 *   -> Call function "fpCounter_p_IRQ" in USER CODE part of your timer's
 *      interrupt handler.
 *	- If you want to use DMA, then no further configuration is needed. Just
 *	  make sure that if you are using LL drivers, DMAx and STREAMy for both
 *	  channels is defined in "fpCounter_Config.h".
 *  - Call function "fpCounter_p_init" to start and enable timer inputs in
 *    your main loop after timer init by MX, so after USER CODE BEGIN 2.
 *  - Call function "fpCounter_demo" to enable PWM generation.
 *  - Call function "fpCounter_p_get" wherever you need to convert new
 *    data for frequency, period and duty cycle.
 *
 *  written by Matej Blagsic (@SausageCode)
 */
#ifndef FPCOUNTER_H
#define FPCOUNTER_H

#include "main.h"
#include "fpCounter_Config.h"

#ifdef HAL_TIM_MODULE_ENABLED
extern TIM_HandleTypeDef FPCOUNTER_TIM_HANDLE;
extern TIM_HandleTypeDef FPCOUNTER_DEMO_HANDLE;
#else
#define FPCOUNTER_TIM_USE_LL
#endif

/* Prototype functions */
HAL_StatusTypeDef fpCounter_p_init(void);
void fpCounter_p_IRQ(void);
void fpCounter_p_get(double *period, uint32_t *frequency, float *duty);
HAL_StatusTypeDef fpCounter_demo(void);

#endif
