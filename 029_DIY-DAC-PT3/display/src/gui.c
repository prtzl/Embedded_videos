#include "gui.h"
#include "ssd1306.h"
#include <stdio.h>
#include <string.h>
#include "timer.h"

#define FONT_DEF Font_7x10
extern SSD1306_t SSD1306;

/** Screen init functions **/
/**
* @brief  Print title for init
**/
void screen_init(void)
{
	// Title
	ssd1306_SetCursor(0, 0); // start position
	ssd1306_WriteString("PCM1792 AUDIO DAC", FONT_DEF, White);
	ssd1306_SetCursor(0, FONT_DEF.FontHeight); // move cursor to new line

	// Init start
	ssd1306_SetCursor(0, SSD1306.CurrentY);
	ssd1306_WriteString("System start", FONT_DEF, White);
	ssd1306_SetCursor(0, SSD1306.CurrentY + FONT_DEF.FontHeight + 4);

	ssd1306_UpdateScreen();
}

/**
* @brief  Print interface init status
**/
void screen_initInterface(INTRError status)
{
	// Print PCM error
	char text[10];
	switch(status)
	{
		case INTROK:
			sprintf(text, "%s", "OK");
			break;
		case ErrorIntrPwr:
			sprintf(text, "%s", "ERR-PWR");
			break;
		case ErrorIntrUsb:
			sprintf(text, "%s", "ERR-USB");
			break;
	}
	ssd1306_SetCursor(0, SSD1306.CurrentY);
	ssd1306_WriteString("INTERFACE:", FONT_DEF, White);
	ssd1306_SetCursor(SSD1306_WIDTH - (FONT_DEF.FontWidth *strlen(text)), SSD1306.CurrentY);
	ssd1306_WriteString(text, FONT_DEF, White);
	ssd1306_SetCursor(0, SSD1306.CurrentY + FONT_DEF.FontHeight);

	ssd1306_UpdateScreen();
}

/**
* @brief  Print PCM init status
**/
void screen_initPCM(PCMError status)
{
	// Print PCM error
	char text[10];
	switch(status)
	{
		case PCMOK:
			sprintf(text, "%s", "OK");
			break;
		case ErrorPcmSpi:
			sprintf(text, "%s", "ERR-SPI");
			break;
		case ErrorPcm:
			sprintf(text, "%s", "ERR-DEV");
			break;
	}
	ssd1306_SetCursor(0, SSD1306.CurrentY);
	ssd1306_WriteString("PCM1792:", FONT_DEF, White);
	ssd1306_SetCursor(SSD1306_WIDTH - (FONT_DEF.FontWidth *strlen(text)), SSD1306.CurrentY);
	ssd1306_WriteString(text, FONT_DEF, White);
	ssd1306_SetCursor(0, SSD1306.CurrentY + FONT_DEF.FontHeight);

	ssd1306_UpdateScreen();
}

/**
* @brief  Print RTOS init status
**/
void screen_initRTOS(ErrorStatus status)
{
	ssd1306_SetCursor(0, SSD1306.CurrentY);
	ssd1306_WriteString("RTOS:", FONT_DEF, White);
	if(status == SUCCESS)
	{
		ssd1306_SetCursor(SSD1306_WIDTH-1-(FONT_DEF.FontWidth*2), SSD1306.CurrentY);
		ssd1306_WriteString("OK", FONT_DEF, White);
	}
	else
	{
		ssd1306_SetCursor(SSD1306_WIDTH-1-(FONT_DEF.FontWidth*3), SSD1306.CurrentY);
		ssd1306_WriteString("ERR", FONT_DEF, White);
	}
	ssd1306_SetCursor(0, SSD1306.CurrentY + FONT_DEF.FontHeight);

	ssd1306_UpdateScreen();
}

/**
* @brief  Print all peripheral statuses
**/
void screen_initStatus(NewError_t *status)
{
	screen_initInterface(status->intr);
	delay_ms(250);
	screen_initPCM(status->pcm);
	delay_ms(250);
	screen_initRTOS(status->rtos);

	// Optional delay to have a better view ot statuses
	delay_ms(5000);

	// If anyone fails, stop
	if( (status->intr || status->pcm || status->rtos) != 0 )
	{
		ssd1306_Fill(Black);
		ssd1306_SetCursor(SSD1306_WIDTH/2, SSD1306_HEIGHT/2);
		ssd1306_WriteString("PENUTS", FONT_DEF, White);
		ssd1306_UpdateScreen();
		while(1);
	}
}

/** USER GUI **/

/**
* @brief  Print start interface
**/
void screen_startDAC(void)
{
	uint32_t i;

	// Clear screen under title
	for(i = SSD1306_WIDTH*FONT_DEF.FontHeight; i < SSD1306_WIDTH*SSD1306_HEIGHT; i++)
	{
		ssd1306_DrawPixel(i%SSD1306_WIDTH, i/SSD1306_WIDTH, Black);
	}

	ssd1306_UpdateScreen();
}

/**
* @brief  Print current time on 2nd row of screen
* @param  tim time in ms
* @info		Can select time from RTOS timer or the selected
* 				system timer with timer_getms();
* 				If you do not have other timer, just use timer_getms();
* 				with argument 0
* 				This function is used by RTOS and is called every second
**/
void screen_clock(uint32_t tim)
{
	char txt[10];
	uint32_t ms, s, m, h;

	// Select between RTOS timer and System delay timer (TIM5)
	if( tim == 0 )
	{
		ms = timer_getms();
	}
	else
	{
		ms = tim *1000;
	}

	h = ms/3600000;
	m = (ms -h*3600000) /60000;
	s = (ms -m*60000 -h*3600000) /1000;

	sprintf(txt, "%02lu:%02lu:%02lu", h, m, s);
	ssd1306_SetCursor(0, FONT_DEF.FontHeight);
	ssd1306_WriteString(txt, FONT_DEF, White);

	ssd1306_UpdateScreen();
}
