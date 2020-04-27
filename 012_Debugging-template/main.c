/*
  This is a template file that you can use to start your project on your
  stm32 developlent board ment for further tutorials in System Workbench.
  Choose the StdPeriph library in your IDE / project.

  Also make sure to have the right settings in system_stm32f4xx.c for input clock.
  Remember to coment out all delay function to speed up debugging.
*/

#include "stm32f4xx.h"

void RCC_Setup();
void GPIO_Setup();

// DELAY prototypes
void delayMS(uint32_t ms);
void delayUS(uint32_t us);

// Global microseconds for delay functions
uint32_t g_nSysTick;

int main(void)
{
  RCC_Setup();
  GPIO_Setup();

  /*
    Config global system timer to make interrupt every 1us -> microsecond counter
  */
  SysTick_Config(SystemCoreClock / 1000000);

  uint16_t i=0;

  while(1)
  {
    GPIO_ToggleBits(GPIOD,  GPIO_Pin_12 |
                            GPIO_Pin_13 |
                            GPIO_Pin_14 |
                            GPIO_Pin_15);
    i+=15;
    delayMS(1000);
  }
}

void RCC_Setup()
{
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA |
                          RCC_AHB1Periph_GPIOB |
                          RCC_AHB1Periph_GPIOD,
                          ENABLE);
}

/* This is setup funciton for basic GPIO */
void GPIO_Setup()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  // LEDs
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStruct);

  // User button
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* User functions */
void delayUS(uint32_t us)
{
  g_nSysTick = 0;
  while(g_nSysTick < us) __NOP();
}

void delayMS(uint32_t ms)
{
  g_nSysTick = 0;
  while(g_nSysTick < (ms*1000)) __NOP();
}

/* Interrupts */
// System timer interrupt
void SysTick_Handler(void)
{
  g_nSysTick++;
}
