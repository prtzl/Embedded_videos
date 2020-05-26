/**
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * https://github.com/4ilo/ssd1306-stm32HAL
 *
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 * https://github.com/afiskon/stm32-ssd1306
 *
 * I took the Aleksander's code and modified it to use LL driver for STM32
 */
/**
 	 This version is modified to work with SPI LL driver
**/

#ifndef SSD1306_H
#define SSD1306_H

#include <stddef.h>
#include <_ansi.h>
#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"
#include "ssd1306_fonts.h"

#define SSD1306_SH1106 // IF SH1106 controller is used (132x64)
#define SSD1306_USE_SPI
//#define SSD1306_USE_I2C // to be implemented

/** HARDWARE SETTINGS **/
#if defined(SSD1306_USE_SPI)
#define SSD1306_SPI_PORT				SPI3

#define SSD1306_CS_Port         GPIOD
#define SSD1306_CS_Pin          LL_GPIO_PIN_0

#define SSD1306_DC_Port         GPIOD
#define SSD1306_DC_Pin          LL_GPIO_PIN_1

#define SSD1306_Reset_Port      GPIOD
#define SSD1306_Reset_Pin       LL_GPIO_PIN_2
#elif defined(SSD1306_USE_I2C)
/**
 * To be implemented
**/
#endif

// SSD1306 OLED height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT          64
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH           128
#endif

/** APP SETTINGS **/
// Enumeration for screen colors
typedef enum
{
	Black = 0x00, // Black color, no pixel
	White = 0x01  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;

// Struct to store transformations
typedef struct
{
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_t;

/** Prototypes **/
// Procedure definitions
void ssd1306_Init(uint8_t contrast);
void ssd1306_Fill(SSD1306_COLOR color);
void ssd1306_UpdateScreen(void);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
char ssd1306_WriteChar(const char ch, FontDef Font, SSD1306_COLOR color);
char ssd1306_WriteString(const char *str, FontDef Font, SSD1306_COLOR color);
void ssd1306_SetCursor(uint8_t x, uint8_t y);

// Low-level procedures
void ssd1306_Reset(void);
void ssd1306_WriteCommand(uint8_t byte);
void ssd1306_Write2Command(uint8_t byte1, uint8_t byte2);
void ssd1306_WriteByte(uint8_t byte);
void ssd1306_WriteData(uint8_t* buffer, size_t buff_size);

#endif
