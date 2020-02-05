/* This code shows how to use USART to receive messages from PC over RS232 USB to Serial modules */
#include "stm32f4xx.h"

// Setup function prototypes
void GPIO_Setup(void);
void USART1_Setup(void);

// Other function prototypes
void USART_SendText(volatile char *s);
void USART_SendNumber(uint32_t x);

int main(void)
{
  GPIO_Setup();
  USART1_Setup();
  while(1)
  {}
}

/* This function sets up GPIO */
void GPIO_Setup()
{
  // Init clock to GPIOB for USART1 and GPIOD for LED
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitTypeDef GPIO_InitStruct;

  // Init LED on GPIOD
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOD, &GPIO_InitStruct);

  // Init GPIOB
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  // Connect pins to AF pins of USART1
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
}

/* This function sets up and starts USART1 and RXNE interrupt */
void USART1_Setup()
{
  // Init clock for USART peripheral
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  // Init USART
  USART_InitTypeDef USART_InitStruct;
  USART_InitStruct.USART_BaudRate = 9600;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No ;
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART1, &USART_InitStruct);
  USART_Cmd(USART1, ENABLE); // Start USART1 peripheral

  // Init USART1 interrupt for when receiving register is not empty
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  NVIC_EnableIRQ(USART1_IRQn); // enable USART1 interrupt
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

/* This is the function that gets executed on USART1 interrupt */
void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE)) // was is triggered by RXNE trigger
  {
    if(USART_ReceiveData(USART1) == 'K') // if letter K was transmitted
    {
      GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
      USART_SendText("LED ON\n"); // send text
      // while( !USART_GetFlagStatus(USART1, USART_FLAG_TXE) );
      // USART_SendData(USART1, 'H');
      // while( !USART_GetFlagStatus(USART1, USART_FLAG_TXE) );
      // USART_SendData(USART1, 'I');
      // while( !USART_GetFlagStatus(USART1, USART_FLAG_TXE) );
      // USART_SendData(USART1, 10);
    }
  }
}
