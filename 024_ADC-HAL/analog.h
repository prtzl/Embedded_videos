#ifndef __ANALOG_H
#define __ANALOG_H

#include "main.h"
extern ADC_HandleTypeDef hadc1;

// Video functions
uint8_t video_adc_getData(uint16_t *data, uint32_t channel);
void video_adc_toMilliVolt(uint16_t *data, uint16_t *voltage);
// Additional functions
HAL_StatusTypeDef adc_getData(ADC_HandleTypeDef* hadc, uint16_t *data, uint32_t channel);
void adc_toVolt(uint16_t *data, float *voltage);

#endif
