#include "analog.h"

/*
Call this function to perform calibration of ADCx peripheral

Input argument is appropriate adc handle
depending on the adc that you are using:
&hadc1, &hadc2 ...
*/

void adc_cal(ADC_HandleTypeDef *hadc)
{
	HAL_StatusTypeDef ret;

	// Make sure ADC peripheral is stopped
	ret = HAL_ADC_Stop(hadc);
	if(ret != HAL_OK)
	{
		Error_Handler();
	}
	
	HAL_Delay(10);

	/* Choose type of measurement
	ADC_SINGLE_ENDED
	ADC_DIFFERENTIAL_ENDED
	*/
	ret = HAL_ADCEx_Calibration_Start(hadc, ADC_SINGLE_ENDED);
	if(ret != HAL_OK)
	{
		Error_Handler();
	}

	// Small delay to ensur end of calibration procedure
	HAL_Delay(10);
}

/*
Call this function to poll adc value

Input argument is appropriate adc handle
depending on the adc that you are using:
&hadc1, &hadc2 ...
*/
uint16_t adc_getVal(ADC_HandleTypeDef *hadc)
{
	HAL_StatusTypeDef ret;

	// Start ADC peripheral
	ret = HAL_ADC_Start(hadc);
  	if(ret != HAL_OK)
  	{
  		Error_Handler();
  	}

  	// Poll for conversion
  	ret = HAL_ADC_PollForConversion(hadc, 50);
  	if(ret != HAL_OK)
  	{
  		Error_Handler();
  	}

  	// Return conversion result
  	return (uint16_t) HAL_ADC_GetValue(hadc);
}