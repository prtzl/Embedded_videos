#include "spi_examples.h"

/**
 * This SPI example is used with PCM1792a DAC.
 * Refere to its datasheet for information on
 * SPI settings, data sizes, etc
**/

/**
* @brief  Writes 1B of data to device register
* @param  regAddr address of register we want to write to
* @param  pData pointer to data to write into device register
* @retval HAL status
**/
HAL_StatusTypeDef spi_write(uint8_t regAddr, uint8_t *pData)
{
  HAL_StatusTypeDef ret;
  uint8_t sendData[2] = {regAddr, *pData};

  // Start communication
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 0);

  // send reg addr and data to write into
  ret = HAL_SPI_Transmit(&hspi1, sendData, 2, 20);
  if(ret != HAL_OK)
  {
  	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);
    return ret;
  }

  // End communication
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);
  return HAL_OK;
}

/**
* @brief  Reads 1B of data from device register
* @param  regAddr address of register we want to write to
* @param  pData pointer to variable to store device register data
* @retval HAL status
**/
HAL_StatusTypeDef spi_read(uint8_t regAddr, uint8_t *pData)
{
  HAL_StatusTypeDef ret;
  // MSB of address is 1 to tell device we want to read data from it
  uint8_t address = regAddr | 0x80;

  // Assemble
  uint8_t senddata[2] = {address, 0};
  uint8_t receivedata[2] = {0};

  // Start communication
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 0);

  /** 1. way: send address to read from and read separate **/
//  // send send reg addr to read from
//  ret = HAL_SPI_Transmit(&hspi1, &address, 1, 20);
//  if(ret != HAL_OK)
//  {
//  	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);
//    return ret;
//  }
//  // read a byte from reg
//  ret = HAL_SPI_Receive(&hspi1, pData, 1, 20);
//  if(ret != HAL_OK)
//  {
//  	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);
//    return ret;
//  }

  /** 2. way (my preferred): send reg address and read with one function
   *	Size of Rx and Tx register is same -> length of whole transmission
  **/
  ret = HAL_SPI_TransmitReceive(&hspi1, senddata, receivedata, 2, 50);
  if(ret != HAL_OK)
  {
  		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);
    return ret;
  }

  // End communication
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);
  return HAL_OK;
}
