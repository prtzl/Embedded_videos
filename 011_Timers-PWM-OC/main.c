/*
  This example shows how to use timer OC functions for toggle or PWM.
  Follow the video for other functions described in between.
*/

#include "stm32f4xx.h"

void RCC_Setup();
void GPIO_Setup();
void USART1_Setup();
void TIMER4_Setup();

// USART prototypes
void USART_SendText(volatile char *s);
void USART_SendNumber(uint32_t x);
// DELAY prototypes
void delayMS(uint32_t ms);
void delayUS(uint32_t us);

// Global microseconds for delay functions
uint32_t g_nSysTick;

int main(void)
{
  RCC_Setup();
  GPIO_Setup();
  USART1_Setup();
  TIMER4_Setup();

  /*
    Config global system timer to make interrupt every 1us -> microsecond counter
  */
  SysTick_Config(SystemCoreClock / 1000000);

  // A structure if you want to check your clock settings
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);

  while(1)
  {
    // Will dimm LEDs with 1000 1ms steps
    for(int i = 1; i<1000;i++)
    {
      //change value of CCRx register directly
      TIM4->CCR3 = i;
      TIM4->CCR4 = 1000 - i;
      delayMS(1);
    }

    for(int i = 1; i<1000;i++)
    {
      TIM4->CCR3 = 1000 - i;
      TIM4->CCR4 = i;
      delayMS(1);
    }
  }
}

void RCC_Setup()
{
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA |
                          RCC_AHB1Periph_GPIOB |
                          RCC_AHB1Periph_GPIOD,
                          ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

/* This is setup funciton for basic GPIO */
void GPIO_Setup()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  // LEDs
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);

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
  USART_InitStruct.USART_StopBits = USART_StopBits_1; //increase this if you have "slow end"
  USART_InitStruct.USART_Parity = USART_Parity_No ; //change if data integrity is important
  USART_InitStruct.USART_Mode = USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART1, &USART_InitStruct);
  USART_Cmd(USART1, ENABLE);

  USART_SendText("\nTESTING\n"); // a test
}

/* This function sets up TIMER4 and its OC registers */
void TIMER4_Setup()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_TimeBaseInitStruct.TIM_Period = 999; // Set PWM resolution with timer period
  TIM_TimeBaseInitStruct.TIM_Prescaler = 83; // Adjust period for PWM frequency
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

  TIM_OCInitTypeDef TIM_OCInitStruct;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;

  // OC1 and OC2 set up for Toggle
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStruct.TIM_Pulse = 0; //trigger at count 0
  TIM_OC1Init(TIM4, &TIM_OCInitStruct);
  TIM_OCInitStruct.TIM_Pulse = 5000;
  TIM_OC2Init(TIM4, &TIM_OCInitStruct);

  // OC3 and OC4 set up for PWM with preload enabled(MUST)
  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;

  TIM_OCInitStruct.TIM_Pulse = 499; // stop at 499 val of period
  TIM_OC3Init(TIM4, &TIM_OCInitStruct);
  TIM_OCInitStruct.TIM_Pulse = 99;
  TIM_OC4Init(TIM4, &TIM_OCInitStruct);

  TIM_Cmd(TIM4, ENABLE); // start timer
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
