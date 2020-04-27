/* This code shows how to use DMA with ADC for a simple and usefull example.
*  We will be sending data over USART
*/
#include "stm32f4xx.h"

// Setup function prototypes
void RCC_Setup();
void GPIO_Setup();
void USART1_Setup();
void ADC1_Setup();
void DMA2_Setup();

// Other function prototypes
void USART_SendText(volatile char *s);
void USART_SendNumber(uint32_t x);

// Global variables
uint16_t adcdata[2];
float temp;

int main(void)
{
  RCC_Setup();
  GPIO_Setup();
  USART1_Setup();
  ADC1_Setup();
  DMA2_Setup();

  while(1)
  {
    temp = adcdata[1] * 3.3 / ((1 << 12)-1); // convert data from temp sensor to volts
    temp -= 0.76;
    temp /= 0.0025; //these two convert it to degrees C
    USART_SendNumber(adcdata[0]); // send potentiometer on PA0 data
    USART_SendText("\t"); // tab
    USART_SendNumber((uint16_t)temp); // send temperature
    USART_SendText("\n"); // new line
    for(long i = 0; i < SystemCoreClock/100; i++){__NOP();} // wait some time
  }
  return 0;
}

/* This function sets up clocks for all peripherals */
void RCC_Setup()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA2, ENABLE);
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
  ADC_InitStruct.ADC_ScanConvMode = ENABLE; // sweep multiple channels
  ADC_InitStruct.ADC_ContinuousConvMode = ENABLE; // continue making conversions
  ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStruct.ADC_NbrOfConversion = 2; // now we convert 2 values aka 2 channels
  ADC_Init(ADC1, &ADC_InitStruct);

  // Configure ADC input channels
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_144Cycles); // input is channel 0
  ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 2, ADC_SampleTime_144Cycles); // temp sensor input
  ADC_TempSensorVrefintCmd(ENABLE); // enable temperature sensor

  // Start ADC-DMA requests
  ADC_DMACmd(ADC1, ENABLE);
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

  // Start ADC
  ADC_Cmd(ADC1, ENABLE); // Start ADC1
  ADC_SoftwareStartConv(ADC1); // Start ADC1 conversions
}

/* This function sets up DMA2 peripheral */
void DMA2_Setup()
{
  DMA_InitTypeDef DMA_InitStruct;

  DMA_InitStruct.DMA_Channel = 0;
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; // address of ADC data
  DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)adcdata; // address of destination
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory; // we copy from periph to memory
  DMA_InitStruct.DMA_BufferSize = 2; // we will copy two values
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; // automatically advance dest. mem. addr.
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //16bit
  DMA_InitStruct.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord; //16bit
  DMA_InitStruct.DMA_Mode = DMA_Mode_Circular; // do it over and over again
  DMA_InitStruct.DMA_Priority = DMA_Priority_Medium; // priority
  DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable; // all default
  DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStruct);
  DMA_Cmd(DMA2_Stream0, ENABLE); // Start DMA2
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
