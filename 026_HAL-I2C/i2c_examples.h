#ifndef __i2c_examples_H
#define __i2c_examples

#include "main.h"

extern I2C_HandleTypeDef hi2c1;

/* All fixed constants are defined here
	 If you have only one of each IC, then
	 you can define its address here to be universal.
	 This file is the same as in video, therefore address,
	 vref and resolution are defined in main
 * */

// Function prototypes
HAL_StatusTypeDef ADS7828_readADC(uint8_t address, uint8_t pinCfg, uint16_t *adcdata);
HAL_StatusTypeDef MAX30205_readTemp(uint8_t dev_address, uint8_t config, float *temp);
void adc_toVolt(float vref, uint8_t res, uint16_t adcval, float *voltage);

#endif
