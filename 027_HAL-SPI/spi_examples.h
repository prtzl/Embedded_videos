#ifndef __spi_examples_H
#define __spi_examples_H

#include "main.h"

extern SPI_HandleTypeDef hspi1;

// Function prototypes
HAL_StatusTypeDef spi_write(uint8_t regAddr, uint8_t *pData);
HAL_StatusTypeDef spi_read(uint8_t regAddr, uint8_t *pData);

#endif
