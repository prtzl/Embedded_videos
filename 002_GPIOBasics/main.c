/* This code shows how to manipulate GPIO ports */

#include "stm32f4xx.h"

int main(void)
{
  // Enable clock to GPIOA and GPIOD
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD, ENABLE);

  // Define and init struct for GPIOD
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStruct);
  // Define and init struct for GPIOA
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  while(1)
  {
    // Check button state and turn LEDs ON if button is pressed
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
    {
      GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    }
    else
    {
      GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    }
    // Blink LED/LEDs in an interval. Uncomment bottom and comment top part of the code
    // GPIO_SetBits(GPIOD, GPIO_Pin_14);
    // for(long i=0; i<SystemCoreClock/30; i++){__NOP();}
    // GPIO_ResetBits(GPIOD, GPIO_Pin_14);
    // for(long i=0; i<SystemCoreClock/30; i++){__NOP();}
  }
  return 0;
}
