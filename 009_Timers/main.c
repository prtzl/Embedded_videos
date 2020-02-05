/*
  This is a template file that you can use to start your project on your
  stm32 developlent board. Use it with the supplied Makefile, stm32_flash.ld,
  stm32f4xx_conf.h and system_stm32f4xx.c files for your need.

  Edit Makefile to include right paths to all stlibs, arm compiler and st-link.

  Also make sure to have the right settings in system_stm32f4xx.c for input clock.
*/

#include "stm32f4xx.h"

void RCC_Setup();
void GPIO_Setup();
void USART1_Setup();
void TIMER3_Setup();

// USART prototypes
void USART_SendText(volatile char *s);
void USART_SendNumber(uint32_t x);
// DELAY prototypes
void delayMS(uint32_t ms);
void delayUS(uint32_t us);

// Global us for delay functions
uint32_t g_nTicks;
uint32_t n;

int main(void)
{
  RCC_Setup();
  GPIO_Setup();
  USART1_Setup();
  TIMER3_Setup();

  RCC_ClocksTypeDef RCC_Clocks;

  RCC_GetClocksFreq(&RCC_Clocks);

  while(1)
  {
    GPIO_ToggleBits(GPIOD,  GPIO_Pin_12 |
                            GPIO_Pin_13 |
                            GPIO_Pin_14 |
                            GPIO_Pin_15);
    USART_SendNumber(n++);
    USART_SendText("HI\n");

    USART_SendNumber(RCC_Clocks.SYSCLK_Frequency);
    USART_SendText("\t");
    USART_SendNumber(RCC_Clocks.HCLK_Frequency);
    USART_SendText("\t");
    USART_SendNumber(RCC_Clocks.PCLK1_Frequency);
    USART_SendText("\t");
    USART_SendNumber(RCC_Clocks.PCLK2_Frequency);
    USART_SendText("\n");
    delayMS(500);
  }
}

void RCC_Setup()
{
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA |
                          RCC_AHB1Periph_GPIOB |
                          RCC_AHB1Periph_GPIOD,
                          ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
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

  // USART1
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7; // choose a pin pair
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
}

/* This is setup funciton for UART debugging */
void USART1_Setup()
{
  USART_InitTypeDef USART_InitStruct;
  USART_InitStruct.USART_BaudRate = 9600; // change this for your case
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No ;
  USART_InitStruct.USART_Mode = USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART1, &USART_InitStruct);
  USART_Cmd(USART1, ENABLE);

  USART_SendText("\nTESTING\n"); // a test
}

/* This is setup funciton for TIMER3 as uS timer for delay functions */
void TIMER3_Setup()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInitStruct.TIM_Period = 83;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  NVIC_EnableIRQ(TIM3_IRQn);
}

/* User functions */
/* This functions send text or char array passed as argument over USART */
void USART_SendText(volatile char *s)
{
  while(*s){
    while( !USART_GetFlagStatus(USART1, USART_FLAG_TXE) ); // wait until data register is empty
    USART_SendData(USART1, *s);
    *s++;
  }
}

/* This function sends numbers up to 32bit over USART */
void USART_SendNumber(uint32_t x)
{
  char value[10]; //a temp array to hold results of conversion
  int i = 0; //loop index

  do
  {
    value[i++] = (char)(x % 10) + '0'; //convert integer to character
    x /= 10;
  } while(x);

  while(i) //send data
  {
    while( !USART_GetFlagStatus(USART1, USART_FLAG_TXE) );
    USART_SendData(USART1, value[--i]);
  }
}

/* Delay functions */
void delayUS(uint32_t us)
{
  TIM_Cmd(TIM3, ENABLE);
  g_nTicks = 0;
  while(g_nTicks < us) __NOP();
  TIM_Cmd(TIM3, DISABLE);
}

void delayMS(uint32_t ms)
{
  TIM_Cmd(TIM3, ENABLE);
  g_nTicks = 0;
  while(g_nTicks < (ms*1000)) __NOP();
  TIM_Cmd(TIM3, DISABLE);
}

/* Interrupts */
void TIM3_IRQHandler(void)
{
  if(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update))
  {
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    g_nTicks++;
  }
}
