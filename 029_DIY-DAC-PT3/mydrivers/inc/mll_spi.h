#ifndef MLL_SPI_H
#define MLL_SPI_H

//#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_spi.h"

ErrorStatus mll_spi_readByte(SPI_TypeDef *SPIx, uint8_t address, uint8_t *byte);
ErrorStatus mll_spi_writeByte(SPI_TypeDef *SPIx, uint8_t byte);
ErrorStatus mll_spi_write2Byte(SPI_TypeDef *SPIx, uint8_t byte1, uint8_t byte2);
ErrorStatus mll_spi_writeData(SPI_TypeDef *SPIx, uint8_t *byte, uint16_t size);

#endif
