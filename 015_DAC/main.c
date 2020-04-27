#include "stm32f4xx.h"
#include "swo.h"
			
#define PINS GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15

uint32_t gTicks;
void delay(uint16_t us);

int main(void)
{
  SysTick_Config(SystemCoreClock / 1000000);

  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA |
  												RCC_AHB1Periph_GPIOD,
                          ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin = PINS;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  DAC_InitTypeDef DAC_InitStruct;
  DAC_InitStruct.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
  DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStruct);
  DAC_Cmd(DAC_Channel_1, ENABLE);

  uint16_t i,k;

  while(1)
  {
  	for(i=0; i<4096; i++)
  	{
  		DAC_SetChannel1Data(DAC_Align_12b_R, i);
  		k = DAC_GetDataOutputValue(DAC_Channel_1);
  		delay(125);
  	}
  	for(i=4095; i>0; i--)
  	{
  	  DAC_SetChannel1Data(DAC_Align_12b_R, i);
  	  k = DAC_GetDataOutputValue(DAC_Channel_1);
  	  delay(125);
   	}
	}
}

void delay(uint16_t us)
{
  gTicks=0;
  while(gTicks < us) __NOP();
}

void SysTick_Handler(void)
{
  gTicks++;
}
