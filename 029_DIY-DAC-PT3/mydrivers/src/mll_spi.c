#include "mll_spi.h"
#include "timer.h"

/**
* @brief  Write byte to SPIx device
* @param  SPIx spi peripheral typedef pointer
* @param  byte byte to send
**/
ErrorStatus mll_spi_writeByte(SPI_TypeDef *SPIx, uint8_t byte)
{
	uint32_t timeout;

	// Send byte
	timeout = timer_getms();
	while( !LL_SPI_IsActiveFlag_TXE(SPIx) )
	{
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			return ERROR;
		}
	}
  LL_SPI_TransmitData8(SPIx, byte);

  // wait for communication to finish
  timeout = timer_getms();
  while(LL_SPI_IsActiveFlag_BSY(SPIx))
  {
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			return ERROR;
		}
  }

  return SUCCESS;
}

/**
* @brief  Write two bytes one after another to SPIx device
* @param  SPIx spi peripheral typedef pointer
* @param  byte1 first byte to send
* @param  byte2 second byte to send
**/
ErrorStatus mll_spi_write2Byte(SPI_TypeDef *SPIx, uint8_t byte1, uint8_t byte2)
{
	uint32_t timeout;

	// Send first byte
	timeout = timer_getms();
	while( !LL_SPI_IsActiveFlag_TXE(SPIx) )
	{
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			return ERROR;
		}
	}
  LL_SPI_TransmitData8(SPIx, byte1);

  // Send second byte
  timeout = timer_getms();
	while( !LL_SPI_IsActiveFlag_TXE(SPIx) )
	{
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			return ERROR;
		}
	}
  LL_SPI_TransmitData8(SPIx, byte2);

  // wait for communication to finish
  timeout = timer_getms();
  while(LL_SPI_IsActiveFlag_BSY(SPIx))
  {
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			return ERROR;
		}
  }

  return SUCCESS;
}

/**
* @brief  Write multiple bytes to SPIx device
* @param  SPIx spi peripheral typedef pointer
* @param  bytes pointer to byte array
* @param  size number of bytes to send
**/
ErrorStatus mll_spi_writeData(SPI_TypeDef *SPIx, uint8_t *bytes, uint16_t size)
{
	uint16_t i;
	uint32_t timeout;

	// Send <size> bytes of data
	for( i = 0; i < size; i++ )
	{
		timeout = timer_getms();
		while( !LL_SPI_IsActiveFlag_TXE(SPIx) )
		{
			// Check for timeout, 10ms
			if( (timeout+10) < timer_getms() )
			{
				return ERROR;
			}
		}
		LL_SPI_TransmitData8(SPIx, bytes[i]);
	}

	// wait for communication to finish
	timeout = timer_getms();
  while(LL_SPI_IsActiveFlag_BSY(SPIx))
  {
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			return ERROR;
		}
  }

  return SUCCESS;
}

/**
* @brief  Read  byte from SPIx device register
* @param  SPIx spi peripheral typedef pointer
* @param  address device register address to read from
* @param  byte pointer to return variable for read byte
**/
ErrorStatus mll_spi_readByte(SPI_TypeDef *SPIx, uint8_t address, uint8_t *byte)
{
	uint32_t timeout;

	// Send reg address to read from
	timeout = timer_getms();
	while( !LL_SPI_IsActiveFlag_TXE(SPIx) )
	{
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			return ERROR;
		}
	}
	LL_SPI_TransmitData8(SPIx, address);

	// Empty input register
	timeout = timer_getms();
	while( !LL_SPI_IsActiveFlag_RXNE(SPIx) )
	{
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			return ERROR;
		}
	}
	LL_SPI_ReceiveData8(SPIx);

	// Send dummy data to get 16bit pulses
	timeout = timer_getms();
	while( !LL_SPI_IsActiveFlag_TXE(SPIx) )
	{
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			return ERROR;
		}
	}
	LL_SPI_TransmitData8(SPIx, 0x00);

	// Get returned data (2nd received byte)
	timeout = timer_getms();
	while( !LL_SPI_IsActiveFlag_RXNE(SPIx) )
	{
		// Check for timeout, 10ms
		if( (timeout+10) < timer_getms() )
		{
			return ERROR;
		}
	}
	*byte = LL_SPI_ReceiveData8(SPIx);

	return SUCCESS;
}
