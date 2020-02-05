#ifndef __ANALOG_H
#define __ANALOG_H

#include "main.h"

/* ADC */
void adc_cal(ADC_HandleTypeDef *hadc);
uint16_t adc_getVal(ADC_HandleTypeDef *hadc);

#endif