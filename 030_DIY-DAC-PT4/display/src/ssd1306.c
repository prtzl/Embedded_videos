#include "ssd1306.h"
#include "mll_spi.h"
#include "timer.h"

#if defined(SSD1306_USE_I2C)

/**
 * To be implemented
**/

#elif defined(SSD1306_USE_SPI)

/* Reset device with RS pin */
void ssd1306_Reset(void)
{
	// CS = High (not selected)
	LL_GPIO_SetOutputPin(SSD1306_CS_Port, SSD1306_CS_Pin);

	// Reset the OLED
	LL_GPIO_ResetOutputPin(SSD1306_Reset_Port, SSD1306_Reset_Pin);
	delay_ms(10);
	LL_GPIO_SetOutputPin(SSD1306_Reset_Port, SSD1306_Reset_Pin);
	delay_ms(10);
}

/* Send a byte to the command register */
void ssd1306_WriteCommand(const uint8_t byte)
{
	LL_GPIO_ResetOutputPin(SSD1306_CS_Port, SSD1306_CS_Pin); // select OLED
	LL_GPIO_ResetOutputPin(SSD1306_DC_Port, SSD1306_DC_Pin); // command
	mll_spi_writeByte(SSD1306_SPI_PORT, byte);
	LL_GPIO_SetOutputPin(SSD1306_CS_Port, SSD1306_CS_Pin); // un-select OLED
}

/* Send a 2 bytes to the command register */
void ssd1306_Write2Command(const uint8_t byte1, const uint8_t byte2)
{
	LL_GPIO_ResetOutputPin(SSD1306_CS_Port, SSD1306_CS_Pin); // select OLED
	LL_GPIO_ResetOutputPin(SSD1306_DC_Port, SSD1306_DC_Pin); // command
	mll_spi_write2Byte(SSD1306_SPI_PORT, byte1, byte2);
	LL_GPIO_SetOutputPin(SSD1306_CS_Port, SSD1306_CS_Pin); // un-select OLED
}

/* Send byte of data */
void ssd1306_WriteByte(const uint8_t byte)
{
	LL_GPIO_ResetOutputPin(SSD1306_CS_Port, SSD1306_CS_Pin); // select OLED
	LL_GPIO_SetOutputPin(SSD1306_DC_Port, SSD1306_DC_Pin); // command
	mll_spi_writeByte(SSD1306_SPI_PORT, byte);
	LL_GPIO_SetOutputPin(SSD1306_CS_Port, SSD1306_CS_Pin); // un-select OLED
}

/* Send bytes of data */
void ssd1306_WriteData(const uint8_t* buffer, size_t buff_size)
{
	LL_GPIO_ResetOutputPin(SSD1306_CS_Port, SSD1306_CS_Pin); // select OLED
	LL_GPIO_SetOutputPin(SSD1306_DC_Port, SSD1306_DC_Pin); // command
	mll_spi_writeData(SSD1306_SPI_PORT, buffer, buff_size);
	LL_GPIO_SetOutputPin(SSD1306_CS_Port, SSD1306_CS_Pin); // un-select OLED
}

#else
#error "You should define SSD1306_USE_SPI or SSD1306_USE_I2C macro"
#endif

/** SSD1306 variables **/
// Screenbuffer
uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
// Screen object
SSD1306_t SSD1306;

// Initialize the oled screen
void ssd1306_Init(uint8_t contrast)
{
	// Enable spi
	if( !LL_SPI_IsEnabled(SPI3) )
	{
		LL_SPI_Enable(SPI3);
	}

	// Reset OLED
	ssd1306_Reset();

	// Wait for the screen to boot
	delay_ms(100);

	// Init OLED
	ssd1306_WriteCommand(0xAE); //display off

	// Set Memory Addressing Mode: Horizontal
	// 00b,Horizontal Addressing Mode;
	// 01b,Vertical Addressing Mode;
	// 10b,Page Addressing Mode (RESET);
	// 11b,Invalid
	ssd1306_Write2Command(0x20, 0x00);


	//	ssd1306_WriteCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
#ifdef SSD1306_MIRROR_VERT
	ssd1306_Write2Command(0xB0, 0xC0);
	//	ssd1306_WriteCommand(0xC0); // Mirror vertically
#else
	ssd1306_Write2Command(0xB0, 0xC8);
	//	ssd1306_WriteCommand(0xC8); //Set COM Output Scan Direction
#endif

	// set low column address; set high column address
	ssd1306_Write2Command(0x00, 0x10);

	// set start line address
	ssd1306_WriteCommand(0x40);

	// set contrast control register: contrast value (8b)
	ssd1306_Write2Command(0x81, contrast);

#ifdef SSD1306_MIRROR_HORIZ
	ssd1306_WriteCommand(0xA0); // Mirror horizontally
#else
	ssd1306_WriteCommand(0xA1); //--set segment re-map 0 to 127 - CHECK
#endif

	// Set screen color mode
#ifdef SSD1306_INVERSE_COLOR
	ssd1306_WriteCommand(0xA7); //--set inverse color
#else
	ssd1306_WriteCommand(0xA6); //--set normal color
#endif

	// Set multiplex ratio.
#if (SSD1306_HEIGHT == 128)
	// Found in the Luma Python lib for SH1106.
	ssd1306_WriteCommand(0xFF);
#else
	ssd1306_WriteCommand(0xA8); //--set multiplex ratio(1 to 64) - CHECK
#endif

#if (SSD1306_HEIGHT == 32)
	ssd1306_WriteCommand(0x1F); //
#elif (SSD1306_HEIGHT == 64)
	ssd1306_WriteCommand(0x3F); //
#elif (SSD1306_HEIGHT == 128)
	ssd1306_WriteCommand(0x3F); // Seems to work for 128px high displays too.
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

	//0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	ssd1306_WriteCommand(0xA4);

	// set display offset: no offset
	ssd1306_Write2Command(0xD3, 0x00);

	// set display clock divide ratio/oscillator frequency: set divide ratio
	ssd1306_Write2Command(0xD5, 0xF0);

	// set pre-charge period
	ssd1306_Write2Command(0xD9, 0x22);

	// set com pins hardware configuration
#if (SSD1306_HEIGHT == 32)
	ssd1306_Write2Command(0xDA, 0x02);
	//	ssd1306_WriteCommand(0x02);
#elif (SSD1306_HEIGHT == 64)
	ssd1306_Write2Command(0xDA, 0x12);
	//	ssd1306_WriteCommand(0x12);
#elif (SSD1306_HEIGHT == 128)
	ssd1306_Write2Command(0xDA, 0x12);
	//	ssd1306_WriteCommand(0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

	// set vcomh: 0x20,0.77xVcc
	ssd1306_Write2Command(0xDB, 0x20);

	ssd1306_WriteCommand(0x8D); //--set DC-DC enable
	ssd1306_WriteCommand(0x14); //
	ssd1306_WriteCommand(0xAF); //--turn on SSD1306 panel

	// Clear screen
	ssd1306_Fill(Black);

	// Flush buffer to screen
	ssd1306_UpdateScreen();

	// Set default values for screen object
	SSD1306.CurrentX = 0;
	SSD1306.CurrentY = 0;

	SSD1306.Initialized = 1;
}

// Fill the whole screen with the given color
void ssd1306_Fill(SSD1306_COLOR color)
{
	uint32_t i;
	for(i = 0; i < sizeof(SSD1306_Buffer); i++)
	{
		SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
	}
}

// Clear display from x1, y1 point forward
void ssd1306_ClearD(uint8_t x1, uint8_t y1, SSD1306_COLOR color)
{
	uint32_t i, P0;

	P0 = y1/8;
	// Start Not-whole page
	if(y1%8 != 0)
	{
		for(i = P0*SSD1306_WIDTH +x1; i < P0*2*SSD1306_WIDTH; i++)
		{
			SSD1306_Buffer[i] = SSD1306_Buffer[i] & ((1<<(y1%8)) -1);
		}
		P0++;
	}
	// Whole pages
	for(i = P0*SSD1306_WIDTH + x1 ; i < sizeof(SSD1306_Buffer); i++)
	{
		SSD1306_Buffer[i] = 0x00;
	}
}

// Write the screenbuffer with changed to the screen from SSD1306_Buffer
void ssd1306_UpdateScreen(void)
{
	// Write data to each page of RAM. Number of pages
	// depends on the screen height:
	//
	//  * 32px   ==  4 pages
	//  * 64px   ==  8 pages
	//  * 128px  ==  16 pages
	for(uint8_t i = 0; i < SSD1306_HEIGHT/8; i++)
	{
		ssd1306_WriteCommand(0xB0 + i); // Set the current RAM page address.
		ssd1306_WriteCommand(0x00);	// set first column
		ssd1306_WriteCommand(0x10);

		// Simple fix for SH1106 controller -> send 4 extra bytes before and after, display is centred in middle
#if defined(SSD1306_SH1106)
		ssd1306_WriteByte(0x00);
		ssd1306_WriteByte(0x00);
#endif
		// Send display page data
		ssd1306_WriteData(&SSD1306_Buffer[SSD1306_WIDTH*i], SSD1306_WIDTH);
#if defined(SSD1306_SH1106)
		ssd1306_WriteByte(0x00);
		ssd1306_WriteByte(0x00);
#endif
	}
}

// Write the screenbuffer with changed to the screen from selected buffer
// Note: F4, 168MHz: 2ms for 1024B of data
// Note: F4, 168MHz: 300us for copying 1024B between two buffers
// -> little advantage for using this function, but why the fuck not (for full-screen images)
void ssd1306_UpdateScreenFrom(const uint8_t *buffer)
{
	for(uint8_t i = 0; i < SSD1306_HEIGHT/8; i++)
	{
		ssd1306_WriteCommand(0xB0 + i); // Set the current RAM page address.
		ssd1306_WriteCommand(0x00);	// set first column
		ssd1306_WriteCommand(0x10);

#if defined(SSD1306_SH1106)
		ssd1306_WriteByte(0x00);
		ssd1306_WriteByte(0x00);
#endif
		// Send display page data
		ssd1306_WriteData(buffer+(SSD1306_WIDTH*i), SSD1306_WIDTH);
#if defined(SSD1306_SH1106)
		ssd1306_WriteByte(0x00);
		ssd1306_WriteByte(0x00);
#endif
	}
}

//    Draw one pixel in the screenbuffer
//    X => X Coordinate
//    Y => Y Coordinate
//    color => Pixel color
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
{
	if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
	{
		// Don't write outside the buffer
		return;
	}

	// Check if pixel should be inverted
	if(SSD1306.Inverted)
	{
		color = (SSD1306_COLOR)!color;
	}

	// Draw in the right color
	if(color == White)
	{
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	}
	else
	{
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

// Draw 1 char to the screen buffer
// ch       => char om weg te schrijven
// Font     => Font waarmee we gaan schrijven
// color    => Black or White
char ssd1306_WriteChar(const char ch, FontDef Font, SSD1306_COLOR color)
{
	uint32_t i, b, j;

	// Check if character is valid
	if (ch < 32 || ch > 126)
	{
		return 0;
	}

	// Check remaining space on current line
	if (SSD1306_WIDTH < (SSD1306.CurrentX + Font.FontWidth) ||
			SSD1306_HEIGHT < (SSD1306.CurrentY + Font.FontHeight))
	{
		// Not enough space on current line
		return 0;
	}

	// Use the font to write
	for(i = 0; i < Font.FontHeight; i++)
	{
		b = Font.data[(ch - 32) * Font.FontHeight + i];
		for(j = 0; j < Font.FontWidth; j++)
		{
			if((b << j) & 0x8000)
			{
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
			}
			else
			{
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
			}
		}
	}

	// The current space is now taken
	SSD1306.CurrentX += Font.FontWidth;
	// Currently not remembering y size ... responsibility of coder !

	// Return written char for validation
	return ch;
}

// Write full string to screen buffer
char ssd1306_WriteString(const char* str, FontDef Font, SSD1306_COLOR color)
{
	// Write until null-byte
	while(*str)
	{
		if(ssd1306_WriteChar(*str, Font, color) != *str)
		{
			// Char could not be written
			return *str;
		}

		// Next char
		str++;
	}

	// Everything ok
	return *str;
}

// Position the cursor
void ssd1306_SetCursor(uint8_t x, uint8_t y)
{
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}
