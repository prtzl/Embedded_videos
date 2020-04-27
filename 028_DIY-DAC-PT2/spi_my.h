#ifndef SPI_MY_H
#define SPI_MY_H

#include "main.h"
#include "timer.h"

// macro for PCM1792 SPI peripheral
#define PCM1792_SPI SPI1

// Prototypes for 
ErrorStatus pcm1792_writeReg(uint8_t regAddr, uint8_t data);
ErrorStatus pcm1792_readReg(uint8_t regAddr, uint8_t *pData);

#endif
