/* This code shows how to use SPI with the onboard accelerometer. We will be sending data over USART */
#include "stm32f4xx.h"

// Setup function prototypes
void RCC_Setup();
void GPIO_Setup();
void USART1_Setup();
void SPI1_Setup();
void SPI_Tx(uint8_t address, uint8_t data);
uint8_t SPI_Rx(uint8_t address);

// Other function prototypes
void USART_SendText(volatile char *s);
void USART_SendNumber(uint32_t x);

// Global variables
uint16_t g_nZAx;

int main(void)
{
  RCC_Setup();
  GPIO_Setup();
  USART1_Setup();
  SPI1_Setup();

  // Config the accelerometer once
  SPI_Tx(0x20, 0x47);

  while(1)
  {
    g_nZAx = SPI_Rx(0x2D); // get Z axis data

    // Twos complement section with if else
    if( ( g_nZAx & 0x80 ) == 0x80 ) // check if data is negative
    {
      // Convert a negative twos complement into absolute value
      g_nZAx = ~g_nZAx;
      g_nZAx += 1;
      g_nZAx &= 0x00FF;
      g_nZAx = ( g_nZAx * 2000 ) / 127;//convert to acceleration in Gs
      USART_SendText("-");// add minus to note it is a negative number
      USART_SendNumber(g_nZAx);
    }
    else
    {
      // Is positive
      g_nZAx = ( g_nZAx * 2300 ) / 127;//convert to acceleration in Gs
      USART_SendNumber(g_nZAx);
    }

    USART_SendText("\n");
    for(long i = 0; i < SystemCoreClock/500; i++){__NOP();} // limit transmit speed
  }
  return 0;
}

/* This function sets up clocks for all peripherals */
void RCC_Setup()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_SPI1, ENABLE);
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

  // Setup GPIOA pins for SPI data and clock lines
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  // Connect pins to AF pins of SPI1
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

  // Setup GPIOE pin for chip select for onboard SPI device
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_3;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOE, &GPIO_InitStruct);
  GPIO_SetBits(GPIOE, GPIO_Pin_3);
}

/* This function sets up and starts USART1 peripheral */
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

/* This function sets up and starts SPI1 peripheral */
void SPI1_Setup()
{
  SPI_InitTypeDef SPI_InitStruct;

  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStruct.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStruct);
  SPI_Cmd(SPI1, ENABLE);
}

/* This function sends data over SPI1 peripheral to a certain register address */
void SPI_Tx(uint8_t address, uint8_t data)
{
  GPIO_ResetBits(GPIOE, GPIO_Pin_3);

  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1, address);
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1);
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1, data);
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1);

  GPIO_SetBits(GPIOE, GPIO_Pin_3);
}

/* This function returns data from a certain address over SPI1 peripheral */
uint8_t SPI_Rx(uint8_t address)
{
  GPIO_ResetBits(GPIOE, GPIO_Pin_3);
  address = 0x80 | address; // this part can be different for your SPI device
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1, address);
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1);
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1, 0x00);
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));

  GPIO_SetBits(GPIOE, GPIO_Pin_3);

  return SPI_I2S_ReceiveData(SPI1);
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
