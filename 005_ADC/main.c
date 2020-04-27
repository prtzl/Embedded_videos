/* This code shows how to use ADC in polling mode and send converted data over USART */
#include "stm32f4xx.h"
#include <stdio.h>

// Setup function prototypes
void RCC_Setup();
void GPIO_Setup();
void USART1_Setup();
void ADC1_Setup();

// Other function prototypes
void USART_SendText(volatile char *s);
void USART_SendNumber(uint32_t x);

float adcdata;
char buffer[10];

int main(void)
{
  RCC_Setup();
  GPIO_Setup();
  USART1_Setup();
  ADC1_Setup();

  while(1)
  {
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); // wati for data
    adcdata = (float)ADC_GetConversionValue(ADC1); // save data
    adcdata = adcdata * 3.3 / (1 << 12); // convert to volts
    sprintf(buffer, "%f", adcdata); // convert float into string
    USART_SendText(buffer); // send data
    USART_SendText("\n"); // send new line command
  }
}

/* This function sets up clocks for all peripherals */
void RCC_Setup()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_ADC1, ENABLE);
}

/* This function sets up GPIO */
void GPIO_Setup()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  // Init LED on GPIOD
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStruct);

  // Init GPIOB pins for USART
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  // Connect pins to AF pins of USART1
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

  // Init GPIOA for analog in for ADC1
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* This function sets up and starts USART1 */
void USART1_Setup()
{
  USART_InitTypeDef USART_InitStruct;
  // Init USART
  USART_InitStruct.USART_BaudRate = 9600;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No ;
  USART_InitStruct.USART_Mode = USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART1, &USART_InitStruct);
  USART_Cmd(USART1, ENABLE); // Start USART1 peripheral

  USART_SendText("TESTING\n");
}

/* This function starts ADC1 peripheral */
void ADC1_Setup()
{
  ADC_CommonInitTypeDef ADC_CommonInitStruct;
  ADC_InitTypeDef ADC_InitStruct;

  ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStruct);

  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStruct.ADC_ScanConvMode = DISABLE;
  ADC_InitStruct.ADC_ContinuousConvMode = ENABLE; // continue making conversions
  ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStruct.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStruct);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_144Cycles); // input is channel 0

  ADC_Cmd(ADC1, ENABLE); // Start ADC1
  ADC_SoftwareStartConv(ADC1); // Start ADC1 conversions
}

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
