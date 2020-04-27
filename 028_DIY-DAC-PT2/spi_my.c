#include "spi_my.h"

/**
* @brief  Write data to PCM1792 register
* @param  regAddr address of register we want to read from
* @param  data input data to write into device register
**/
ErrorStatus pcm1792_writeReg(uint8_t regAddr, uint8_t data)
{
	uint32_t timeout;
	/** Start SPI **/
	LL_GPIO_ResetOutputPin(SPI1_CS_GPIO_Port, SPI1_CS_Pin);

	// Send address and data
	timeout = timer_getms(); // get current time
	while( !LL_SPI_IsActiveFlag_TXE(PCM1792_SPI) )
	{
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			LL_GPIO_SetOutputPin(SPI1_CS_GPIO_Port, SPI1_CS_Pin);
			return ERROR;
		}
	}
	LL_SPI_TransmitData16(PCM1792_SPI, (regAddr << 8) | data); // send register address

	// Dummy read data to clear SPI DR
	timeout = timer_getms(); // get current time
	while( !LL_SPI_IsActiveFlag_RXNE(PCM1792_SPI) )
	{
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			LL_GPIO_SetOutputPin(SPI1_CS_GPIO_Port, SPI1_CS_Pin);
			return ERROR;
		}
		timeout++;
	}
	LL_SPI_ReceiveData16(SPI1);

	/** Stop SPI **/
	LL_GPIO_SetOutputPin(SPI1_CS_GPIO_Port, SPI1_CS_Pin);
	return SUCCESS;
}

/**
* @brief  Read data from PCM1792 register
* @param  regAddr address of register we want to read from
* @param  pData data pointer for device register contents
* @retval ErrorStatus
**/
ErrorStatus pcm1792_readReg(uint8_t regAddr, uint8_t *pData)
{
	uint32_t timeout;
	uint16_t outData; // transmit command
	regAddr |= 0x80; // read mode
	// create 16b transfer data with last 8b as dummy for receive data clock
	outData = (regAddr<<8) & 0xFF00;

	// Start SPI communication
	LL_GPIO_ResetOutputPin(SPI1_CS_GPIO_Port, SPI1_CS_Pin);

	// Send register address data
	timeout = timer_getms(); // get current time
	while( !LL_SPI_IsActiveFlag_TXE(PCM1792_SPI) )
	{
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			LL_GPIO_SetOutputPin(SPI1_CS_GPIO_Port, SPI1_CS_Pin);
			return ERROR;
		}
		timeout++;
	}
	timeout = 0;
	LL_SPI_TransmitData16(PCM1792_SPI, outData);

	// Read from register when data is available
	timeout = timer_getms(); // get current time
	while( !LL_SPI_IsActiveFlag_RXNE(PCM1792_SPI) )
	{
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			LL_GPIO_SetOutputPin(SPI1_CS_GPIO_Port, SPI1_CS_Pin);
			return ERROR;
		}
		timeout++;
	}
	if( LL_SPI_IsActiveFlag_OVR(PCM1792_SPI) )
	{
		// check overrun error
		LL_GPIO_SetOutputPin(SPI1_CS_GPIO_Port, SPI1_CS_Pin);
		return ERROR;
	}
	// only last 8b is meaning full, so no problem casting to 8b
	*pData = (uint8_t)LL_SPI_ReceiveData16(PCM1792_SPI);

	// Stop spi communication
	LL_GPIO_SetOutputPin(SPI1_CS_GPIO_Port, SPI1_CS_Pin);

	return SUCCESS;
}
