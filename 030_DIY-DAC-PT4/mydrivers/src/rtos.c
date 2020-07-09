#include "rtos.h"
#include "rtos_tasks.h"

uint32_t rtos_slice_us;

/**
* @brief  Init RTOS timers and interrupts
* @param  DEV hadle for PCM1792
**/
ErrorStatus rtos_init(uint32_t slice_us)
{
  /**
    return value:
      0: OK
      !0: error
  **/

  /** Setup SysTick
      Enable SysTick exception (cpu interrupt, not peripheral)
      Setup SysTick freq to be slower than CPU to have larger
      time slices possible before timer goes around
      Calculate timer count value that derments slice length
  **/
  uint32_t ticks = (uint32_t)(slice_us * (SystemCoreClock/1000000 /8)) -1;
  // Is slice length too long?
  if(ticks >= 1<<24)
  {
    // SysTick is only 24bit timer
    return ERROR;
  }
  /**
   * Configure SysTick
  **/
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // stop SysTickto
  // Clock from /8 from coret -> 168MHz /8 = 21MHz => Nper = (Tper(uS) *21) -1
  SysTick->LOAD = ticks; //(nmax = 2^24 = 16 777 216)
  SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk; // enable div by 8 clk from SysClock
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; // enable interrupt
  
  // enable interrupt
  NVIC_EnableIRQ(SysTick_IRQn);
  NVIC_SetPriority(SysTick_IRQn, 0);

  // if all goes well, pass rtos time slice to global variable 
  rtos_slice_us = slice_us;
  return SUCCESS;
}

/*
 * RTOS SCHEDULER IS IN SysTick_Handler()
 * in Core->Src->stm32f4xx_it.c
 */

/**
* @brief  Enable RTOS by starting SysTick
* @param
**/
void rtos_enable(void)
{
  // Masks defined in CMSIS by ARM
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; 
}


/**
* @brief  Enable RTOS by stopping SysTick
* @param
**/
void rtos_disable(void)
{
  // Masks defined in CMSIS by ARM
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
