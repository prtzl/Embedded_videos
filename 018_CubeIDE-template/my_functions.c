#include "my_functions.h"

/*!
 * \brief Sends a character over SWO to the host
 *        Default port is 0
 * \param c Character to send
 */
void SWO_PrintChar(char c)
{
  volatile int timeout;
  uint8_t portNo = 0;

  /* Check if Trace Control Register (ITM->TCR at 0xE0000E80) is set */
  if ((ITM->TCR&ITM_TCR_ITMENA_Msk) == 0) { /* check Trace Control Register if ITM trace is enabled*/
    return; /* not enabled? */
  }
  /* Check if the requested channel stimulus port (ITM->TER at 0xE0000E00) is enabled */
  if ((ITM->TER & (1ul<<portNo))==0) { /* check Trace Enable Register if requested port is enabled */
    return; /* requested port not enabled? */
  }
  timeout = 5000; /* arbitrary timeout value */
  while (ITM->PORT[portNo].u32 == 0) {
    /* Wait until STIMx is ready, then send data */
    timeout--;
    if (timeout==0) {
      return; /* not able to send */
    }
  }
#if 0 /* different version for Cortex-M3 needed? */
  ITM->PORT[portNo].u16 = 0x08 | (c<<8);
#else
  ITM_STIM_U8 = c; /* send data */
#endif
}

/*!
 * \brief Sends a string over SWO to the host
 * \param s String to send
 */
void SWO_PrintString(const char *s)
{
  while (*s!='\0') {
    SWO_PrintChar(*s++);
  }
}

/*!
 * \brief Sends a number over SWO to the host
 * \param n up to 32 bit number to send
 */
void SWO_PrintNumber(uint32_t x)
{
  char value[10],val2[10];
  uint16_t i = 0; //loop index
  uint16_t k = 0;

  do
  {
    value[i++] = (char)(x % 10) + '0'; //convert integer to character
    x /= 10;
  } while(x);

  do
  {
    val2[k++] = value[--i]; //flip order to send msbfirst
  } while(i);

  for(uint8_t i = 0; i < k; i++)
  {
    SWO_PrintChar(val2[i]);
  }
}

/*!
 * \brief Sends data to accelerometer register using hspi peripheral
 * \param hspi SPI_HandleTypeDef for periphera
 * \param address spi device register address to send data to
 * \param data data to send
 */
void SPI_Tx(SPI_HandleTypeDef * hspi, uint8_t * address, uint8_t * data)
{
  HAL_StatusTypeDef ret;
  /* Use ret to call Error_Handler() to find error in peripheral initialization
   * Remove this if tested OK
   * */

  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);// start transmission

  ret = HAL_SPI_Transmit(hspi, address,  1, HAL_MAX_DELAY);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }

  ret = HAL_SPI_Transmit(hspi, data,  sizeof(*data), HAL_MAX_DELAY);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }

  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_SET); // stop transmission
}

/*!
 * \brief Receive data from accelerometer register using hspi peripheral
 * \param hspi SPI_HandleTypeDef for periphera
 * \param address spi device register address to read data from
 * \param data variable to write data to
 */
void SPI_Rx(SPI_HandleTypeDef * hspi, uint8_t * address, uint8_t * data, uint8_t dataSize)
{
  *address |= 0x80;// MSB 1 for write mode --> datasheet
  HAL_StatusTypeDef ret;
  /* Use ret to call Error_Handler() to find error in peripheral initialization
   * Remove this if tested OK
   * */

  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);// start transmission

  ret = HAL_SPI_Transmit(hspi, address,  1, HAL_MAX_DELAY);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }

  ret = HAL_SPI_Receive(hspi, data, dataSize, HAL_MAX_DELAY);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }

  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_SET); // stop transmission
}
