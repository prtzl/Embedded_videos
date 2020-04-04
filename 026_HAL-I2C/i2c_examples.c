#include "i2c_examples.h"

/**
* @brief  Gets adc data from ADS7828
* @param  address ADS7828 address set by A0 and A1 pins(default:00 = 0x48)
* @param  pinCfg configuration of SD and C[2..0] bits for
* 				input pin selection
* 				SD: 0 for differential, 1 for single ended
* 				C[2..0] input pin configuration
* 				by default, internal reference and ADC is turned on(bits PD[1..0])
* @param	adcdata pointer to target memory for adc data
* @retval HAL status
*/
HAL_StatusTypeDef ADS7828_readADC(uint8_t address, uint8_t pinCfg, uint16_t *adcdata)
{
	HAL_StatusTypeDef ret;
	uint8_t addata[2];

	// Send configuration register data
	ret = HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(address<<1), &pinCfg, 1, 50);
	if(ret != HAL_OK)
	{
		return ret;
	}

	for(int i=0; i<5000; i++);

	// Receive voltage data, two bytes
	ret = HAL_I2C_Master_Receive(&hi2c1, (uint16_t)(address<<1)|0x01, addata, 2, 50);
	if(ret != HAL_OK)
	{
		return ret;
	}

	// Assemble adc reading data from two bytes
	*adcdata = ((addata[0] & 0x0F) << 8) | addata[1];
	return HAL_OK;
}

/**
* @brief  Get temperature from MAX30205
* @param  dev_address MAX30205 address set by A0,A1, A2 pins
* 				datasheet provides 8bit address, therefore no need
* 				to left shift in i2c functions (default=000 or 0x90 = (0x48 <<1))
* @param  config configuration register for MAX30205 (size 8b)
* 				configuration register address is 0x01
* @param	temp pointer to target memory for temperature data
* @retval HAL status
*/
HAL_StatusTypeDef MAX30205_readTemp(uint8_t dev_address, uint8_t config, float *temp)
{
	HAL_StatusTypeDef ret;
	uint8_t tempData[2];

	// Send configuration, reg 1
	ret = HAL_I2C_Mem_Write(&hi2c1, (uint16_t)(dev_address), 0x01, 1, &config, 1, 50);
	if(ret != HAL_OK)
	{
		return ret;
	}

	for(int i=0; i<5000; i++);

	// Get temperature data, reg 0, two bytes
	ret = HAL_I2C_Mem_Read(&hi2c1, (uint16_t)(dev_address) |0x01, 0x00, 1, tempData, 2, 50);
	if(ret != HAL_OK)
	{
		return ret;
	}

	// Convert to temperature
	// Datasheet shows that digits are powers of two in temperature degree C
	*temp = ( (tempData[0] << 8) | tempData[1]) *0.00390625;

	return HAL_OK;
}

/**
* @brief  Convert adc 16bit value to float voltage
* @param	vref reference voltage of adc(internal?)
* @param  adcval  value of adc to convert
* @param  voltage converted adc value
* @retval HAL status
*/
void adc_toVolt(float vref, uint8_t res, uint16_t adcval, float *voltage)
{
	*voltage = adcval * vref / (float)((1<<res) -1);
}
