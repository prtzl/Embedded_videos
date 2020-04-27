#ifndef __my_functions_H
#define __my_functions_H

#include "main.h"

/* SWO Definitions */
#define ITM_STIM_U32 (*(volatile unsigned int*)0xE0000000)    // Stimulus Port Register word acces
#define ITM_STIM_U8  (*(volatile         char*)0xE0000000)    // Stimulus Port Register byte acces
#define ITM_ENA      (*(volatile unsigned int*)0xE0000E00)    // Trace Enable Ports Register
#define ITM_TCR      (*(volatile unsigned int*)0xE0000E80)    // Trace control register

/* SWO Functions */
void SWO_PrintChar(char c);
void SWO_PrintString(const char *s);
void SWO_PrintNumber(uint32_t x);

/* SPI Functions */
void SPI_Tx(SPI_HandleTypeDef * hspi, uint8_t * address, uint8_t * data);
void SPI_Rx(SPI_HandleTypeDef * hspi, uint8_t * address, uint8_t * data, uint8_t dataSize);

#endif /* __my_functions_H */
