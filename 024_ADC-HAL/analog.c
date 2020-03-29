/*
	This code is an example for VIDEO24 on ADC in HAL.
	All the functions were discussed in the video.
	You can copy this functions to your own code or
	add these files to your project as sources(
	make sure you add "analog.h" to your main)
*/

#include "analog.h"

/*
	These functions were used in the videos
	For additional function, go to tho the bottom of this file
*/
// Get adc data
// Arguments: adc data pointer, channel name
// Returns: 0 if function executed with no problems
//					1 if ADC could not start
//					2 if could not poll for conversion (timeout?)
//					3 if could not config channel 
uint8_t video_adc_getData(uint16_t *data, uint32_t channel)
{
	HAL_StatusTypeDef ret;
	ADC_ChannelConfTypeDef sConfig = {0};

  sConfig.Channel = channel;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    return 3;
  }

	ret = HAL_ADC_Start(&hadc1);
	if(ret != HAL_OK)
	{
		return 1;
	}

	ret = HAL_ADC_PollForConversion(&hadc1, 20);
	if(ret != HAL_OK)
	{
		return 2;
	}

	*data = HAL_ADC_GetValue(&hadc1);
	return 0;
}

// Convert adc data to millivolts
// Arguments: adc data pointer, adc voltage pointer
void video_adc_toMilliVolt(uint16_t *data, uint16_t *voltage)
{
	*voltage = *data * (3300 / 4095);
}

/*
	These files are alternatives and additions to above ones
*/
// Get adc data
// Arguments: ADC handle pointer, adc data pointer, channel name
// Returns: HAL_StatusTypeDef of hadcx 
HAL_StatusTypeDef adc_getData(ADC_HandleTypeDef* hadc, uint16_t *data, uint32_t channel)
{
	HAL_StatusTypeDef ret;
	ADC_ChannelConfTypeDef sConfig = {0};

  sConfig.Channel = channel;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  ret = HAL_ADC_ConfigChannel(&hadc1, &sConfig);
  if (ret != HAL_OK)
  {
    return ret;
  }

	ret = HAL_ADC_Start(hadc);
	if(ret != HAL_OK)
	{
		return ret;
	}

	// Change the timeout to your needs, Default:10
	ret = HAL_ADC_PollForConversion(hadc, 10);
	if(ret != HAL_OK)
	{
		return ret;
	}

	*data = HAL_ADC_GetValue(hadc);
	return HAL_OK;
}

// Convert adc data to volts
// Arguments: adc data pointer, adc voltage pointer
void adc_toVolt(uint16_t *data, float *voltage)
{
	*voltage = *data * (3.3 / 4095.0);
}
