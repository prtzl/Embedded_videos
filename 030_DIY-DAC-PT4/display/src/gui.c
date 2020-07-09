#include "gui.h"
#include <stdio.h>
#include <string.h>
#include "timer.h"
#include "ssd1306.h"
#include "gui_actions.h"

extern SSD1306_t SSD1306; // Display handle
extern uint8_t a_led_flag; // Action LED flag

// Status/Settings displays
#define SCREEN_TIMEOUT	5
uint8_t status_timeout;

// MENU
#define MENU_START_Y				10
#define MENU_MAX_PAGE_SIZE	5 // limited by screen height (TITLE HEIGHT + remaining/font_height)
#define MENU_PAGE_SIZE			5 // items per page
#if MENU_PAGE_SIZE > MENU_MAX_PAGE_SIZE
#error "SMALLER MENU PAGE SIZE!"
#endif
#define MENU_PAGE_NO				(MENU_ITEM_NO +1 -MENU_PAGE_SIZE) // number of pages
// SUBMENU
#define SUBMENU_START_Y					(MENU_START_Y+10)
#define SUBMENU_MAX_PAGE_SIZE		MENU_PAGE_SIZE
#define SUBMENU_PAGE_SIZE				4
#if SUBMENU_PAGE_SIZE > SUBMENU_MAX_PAGE_SIZE
#error "SMALLER SUB-MENU PAGE SIZE!"
#endif

// MENU ITEMS -> belongs to MENU
const char menu_names[MENU_ITEM_NO][MENU_NAME_LEN] =
{
		{"MUTE"},
		{"DATA FORMAT"},
		{"OUTPUT PHASE"},
		{"OVER-SAMPLING"},
		{"FILTER ROLLOFF"},
		{"DEEMPH FILTER"},
		{"EXIT"}
};
// SUB-MENU ITEMS FOR EACH MENU ITEM -> belongs to SUB-MENU
const char submenu_names[MENU_ITEM_NO][SUBMENU_ITEM_NO][SUBMENU_NAME_LEN] =
{
		{{"ON"}, {"(OFF)"}},
		{{"16b_Right"}, {"20b_Right"}, {"24b_Right"}, {"MSB_24b_Left"}, {"I2S_16b"}, {"(I2S_24b)"}},
		{{"(NORMAL)"}, {"INVERSE"}},
		{{"32"}, {"(64)"}, {"128"}},
		{{"(SHARP)"}, {"SOFT"}},
		{{"(DISABLE)"}, {"32kHz"}, {"44.1kHz"}, {"48kHz"}},
};
// SUB-MENU items default option positions -> to init cursor and page
const uint8_t submenu_defPos[MENU_ITEM_NO] =
{
		1,
		5,
		0,
		1,
		0,
		0,
};

// SUB-MENU function pointers
const pFunction submenu_functions[MENU_ITEM_NO][SUBMENU_ITEM_NO] =
{
		{actionPCM_mute, actionPCM_unmute},
		{actionFMT0, actionFMT1, actionFMT2, actionFMT3, actionFMT4, actionFMT5},
		{actionREV_normal, actionREV_inv},
		{actionOS_32, actionOS_64, actionOS_128},
		{actionsFLT_sharp, actionsFLT_slow},
		{actionsDMF_disable, actionsDMF_32k, actionsDMF_44k1, actionsDMF_48k},
};

// MENU / SUB-MENU VARIABLES
MenuItem_t menu_list[MENU_ITEM_NO];			// Menu list of items with specifications
MENU_t g_menu, submenu[MENU_ITEM_NO];		// menu / sub-menu handles
MENU_t *submenu_current; 								// pointer to current sub-menu
MENU_t *menu_current;
pFunction submenu_function;							// pointer to current sub-menu function
// Last initial sub-menu cursor/page when entering sub-menu
uint8_t submenu_cursor[MENU_ITEM_NO];
uint8_t submenu_page[MENU_ITEM_NO];


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
	ssd1306_WriteString("System", Font_11x18, White);
	ssd1306_SetCursor(0, SSD1306.CurrentY +Font_11x18.FontHeight);
	ssd1306_WriteString("Start", Font_11x18, White);
	ssd1306_SetCursor(0, SSD1306.CurrentY +  +Font_11x18.FontHeight);

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
	ssd1306_SetCursor(0, 18);
	ssd1306_WriteString("INTERFACE:", FONT_DEF, White);
	ssd1306_SetCursor(SSD1306_WIDTH - (FONT_DEF.FontWidth *strlen(text)), SSD1306.CurrentY);
	ssd1306_WriteString(text, FONT_DEF, White);
	ssd1306_SetCursor(0, SSD1306.CurrentY + FONT_DEF.FontHeight);
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
}

/**
* @brief  Print all peripheral statuses
**/
void screen_initStatus(NewError_t *status)
{
	// If anyone fails, stop
	if( (status->intr || status->pcm || status->rtos) != 0 )
	{
		// Clear screen under title
		ssd1306_ClearD(0, 0, Black);
		// Print error message
		ssd1306_SetCursor(0, 0);
		ssd1306_WriteString("Init failed", Font_11x18, White);

		screen_initInterface(status->intr);
		screen_initPCM(status->pcm);
		screen_initRTOS(status->rtos);
		ssd1306_UpdateScreen();
	}

	// Print back title when testing
#ifdef TESTING
	ssd1306_ClearD(0, 0, Black);
	ssd1306_SetCursor(0, 0); // start position
	ssd1306_WriteString("PCM1792 AUDIO DAC", FONT_DEF, White);
	delay_ms(2000);
#endif
}

/** USER GUI **/

/** GUI tools **/
/**
* @brief  Print menu page
**/
static void menu_printMenuPage(MENU_t *menu)
{
	uint32_t i, j, k;
	uint32_t START_Y;
	if( menu->issubmen == 1 )
	{
		START_Y = SUBMENU_START_Y;
	}
	else
	{
		START_Y = MENU_START_Y;
	}

	// Clear page
	for(k = MENU_START_Y; k < SSD1306_HEIGHT; k++) // clear whole menu page, main too
	{
		for(j = FONT_DEF.FontWidth; j < SSD1306_WIDTH; j++)
		{
			ssd1306_DrawPixel(j, k, Black);
		}
	}
//	ssd1306_ClearD(7, 10, Black);

	// Print title
	if( menu->issubmen == 1 )
	{
		ssd1306_SetCursor(0, MENU_START_Y);
		ssd1306_WriteString(menu_list[g_menu.page+g_menu.cursor].name, FONT_DEF, White);
	}
	// Print page
	ssd1306_SetCursor(7, START_Y);
	for(i = 0; i < menu->page_size+1; i++)
	{
		// Print sub-menu
		if(menu->issubmen == 1)
		{
			// If null item, end of list
			if(menu_list[menu->submenpos].options[menu->page +i][0] == 0)
			{
				break;
			}
			ssd1306_WriteString(menu_list[menu->submenpos].options[menu->page +i], FONT_DEF, White);
		}
		// Print menu
		else
		{
			// If null item, end of list
			if(menu_list[menu->page +i].name[0] == 0)
			{
				break;
			}
			ssd1306_WriteString(menu_list[menu->page +i].name, FONT_DEF, White);
		}
		ssd1306_SetCursor(7, SSD1306.CurrentY +FONT_DEF.FontHeight);
	}
	ssd1306_UpdateScreen();
}

/**
* @brief  Print cursor
**/
static void menu_printCursor(MENU_t *menu)
{
	uint32_t j, k;
	uint32_t START_Y;
	if( menu->issubmen == 1)
	{
		START_Y = SUBMENU_START_Y;
	}
	else
	{
		START_Y = MENU_START_Y;
	}

	// Clear cursors
	for(k = START_Y; k < SSD1306_HEIGHT; k++) // clear whole column, menu as well
	{
		for(j = 0; j < FONT_DEF.FontWidth; j++)
		{
			ssd1306_DrawPixel(j, k, Black);
		}
	}

	// Print cursor
	ssd1306_SetCursor(0, START_Y +menu->cursor *FONT_DEF.FontHeight);
	ssd1306_WriteChar('>', FONT_DEF, White);
	ssd1306_UpdateScreen();
}

/**
* @brief  Move cursor UP in interface
**/
void move_cursor_up(MENU_t *menu)
{
	// If already at start
	if(menu->cursor == 0)
	{
		menu->cursor_limUP = 1;
		return;
	}
	else
	{
		// reset limits
		menu->cursor_limDOWN = 0;
		menu->cursor_limUP = 0;
	}

	menu->cursor -= 1;
	menu_printCursor(menu);
}

/**
* @brief  Move cursor DOWN in interface
**/
void move_cursor_down(MENU_t *menu)
{
	// If already at the end
	if(menu->cursor == (menu->page_size-1))
	{
		menu->cursor_limDOWN = 1; // set down limit
		return;
	}
	// Sub-menu dynamic action size
	else if( menu->issubmen && ( (menu->cursor+menu->page) >= (menu_list[menu->submenpos].noitems -1) ) )
	{
		menu->cursor_limDOWN = 1;
		return;
	}
	else
	{
		// reset limits
		menu->cursor_limUP = 0;
		menu->cursor_limDOWN = 0;
	}

	menu->cursor += 1;
	menu_printCursor(menu);
}

/**
* @brief  Move page UP when cursor goes beyond page size
**/
void move_page_up(MENU_t *menu)
{
	if( menu->page == 0 )
	{
		return;
	}

	menu->page -= 1;
	menu_printMenuPage(menu);
}

/**
* @brief  Move page DOWN when cursor goes beyond page size
**/
void move_page_down(MENU_t *menu)
{
	if( (menu->page == (menu->nopages-1)) )
	{
		return;
	}
	menu->page += 1;
	menu_printMenuPage(menu);
}

/** GUI routine **/

/**
* @brief  Print start interface
**/
void screen_start(void)
{
	uint32_t i, j, k;
	// Set initial menu_current pointer to main menu
	menu_current = &g_menu;

	// Set MENU attributes
	g_menu.page_size = MENU_PAGE_SIZE;
	g_menu.nopages = MENU_PAGE_NO;
	g_menu.issubmen = 0;

	// Set MENU
	for(i = 0; i < MENU_ITEM_NO; i++)
	{
		// Set SUB-MENU attributes
		submenu[i].page_size = SUBMENU_PAGE_SIZE;
		submenu[i].issubmen = 1;
		submenu[i].submenpos = i;

		// Set initial cursor and page -> check if default item is not on page0
		if( submenu_defPos[i] >= SUBMENU_PAGE_SIZE )
		{
			submenu[i].cursor = SUBMENU_PAGE_SIZE -1; // set cursor to last place
			submenu[i].exCursor = submenu[i].cursor;
			submenu[i].page = submenu_defPos[i] -SUBMENU_PAGE_SIZE +1;
			submenu[i].exPage = submenu[i].page;
		}
		else
		{
			submenu[i].cursor = submenu_defPos[i]; // set cursor to last place
			submenu[i].exCursor = submenu[i].cursor;
			submenu[i].page = 0;
			submenu[i].exPage = submenu[i].page;
		}

		// Set MENU items
		sprintf(menu_list[i].name, "%s", menu_names[i]); // menu item names

		// Get number of active items (number of items <= SUBMENU_ITEM_NO)
		k = 0;
		while( (submenu_names[i][k][0] != 0) && (k < SUBMENU_ITEM_NO) )
		{
			k += 1;
		}
		menu_list[i].noitems = k;

		// Set sub-menu item names and functions
		for(j = 0; j < menu_list[i].noitems; j++)
		{
			sprintf(menu_list[i].options[j], "%s", submenu_names[i][j]);
			menu_list[i].actions[j] = submenu_functions[i][j];
		}

		// Add back button
		sprintf(menu_list[i].options[menu_list[i].noitems], "%s", "Back");
		menu_list[i].noitems += 1; // add Back option

		// Set number of pages
		if( (menu_list[i].noitems <= SUBMENU_PAGE_SIZE) )
		{
			submenu[i].nopages = 1;
		}
		else
		{
			submenu[i].nopages = (menu_list[i].noitems -SUBMENU_PAGE_SIZE +1);
		}
	}

	// Clear screen under title
	for(i = SSD1306_WIDTH*FONT_DEF.FontHeight; i < SSD1306_WIDTH*SSD1306_HEIGHT; i++)
	{
		ssd1306_DrawPixel(i%SSD1306_WIDTH, i/SSD1306_WIDTH, Black);
	}
}

/**
* @brief  Toggle OK selection
**/
void menu_selection(uint8_t submen)
{
	// Go into menu, select sub-menu item
	if( submen == 1 )
	{
		// Select sub-menu
		menu_current = &submenu[menu_current->cursor+menu_current->page];
		//Print new sub-menu page
		menu_printCursor(menu_current);
		menu_printMenuPage(menu_current);
	}
	// Go back to main menu
	else
	{
		// Perform 'OK' function
		// Check if valid pointer function
		submenu_function = menu_list[g_menu.cursor+g_menu.page].actions[menu_current->cursor + menu_current->page];
		if( submenu_function != 0 )
		{
			submenu_function();
			a_led_flag = 1;
		}
		// Is BACK option? -> reset cursors
		if( menu_current->cursor+menu_current->page+1 == menu_list[menu_current->submenpos].noitems )
		{
			menu_current->cursor = menu_current->exCursor;
			menu_current->page = menu_current->exPage;
		}
		// Unfuck selected sub-menu
		menu_current = &g_menu;
		menu_printCursor(menu_current);
		menu_printMenuPage(menu_current);
	}
}

// Toggle between status screen and setup screen
uint8_t screen_toggle = 0;
/**
* @brief  Show interface/option display
* @info		Toggle between menu/sub-menu
* 				and execute sub-menu commands
* 				This function is called fast in main
* 				to catch button events. It is overiden
* 				by status display operated by rtos
* @param	up scroll up command
* @param	down scroll down command
* @param	ok enter/choose/toggle
**/
void display_settings(uint8_t up, uint8_t down, uint8_t ok)
{
	// Start by entering to menu screen on any button press
	if( (up | down | ok) && (screen_toggle == 0) )
	{
		screen_toggle = 1; // set flag to be in settings
		status_timeout = 0; // reset timeout
		// print start display
		ssd1306_ClearD(0, 10, Black);
		menu_printMenuPage(&g_menu);
		menu_printCursor(&g_menu);
		// toggle to menus
	}
	// Go into settings menu -> next time after entry, not right away
	else if(screen_toggle == 1)
	{
		status_timeout = 0; // reset timeout

		// Remember initial cursor/page to return to
		if( menu_current->exFlag == 0 )
		{
			menu_current->exCursor = menu_current->cursor;
			menu_current->exPage = menu_current->page;
			menu_current->exFlag = 1;
		}

		/** ADVANCE CURSOR **/
		// Only one at once
		if( up == 1 )
		{
			move_cursor_up(menu_current);
		}
		else if( down == 1 )
		{
			move_cursor_down(menu_current);
		}

		/** ADVANCE PAGE **/
		// Only one at once
		if( (menu_current->cursor_limUP == 1) && ( up == 1 ) )
		{
			move_page_up(menu_current);
		}
		else if( (menu_current->cursor_limDOWN == 1) && (down == 1) )
		{
			move_page_down(menu_current);
		}

		/** DEAL WITH OK BUTTON **/
		if( ok == 1 )
		{
			// Enter sub-menu
			if(menu_current->issubmen == 0)
			{
				// Is exit option?
				if( menu_current->cursor+menu_current->page+1 == MENU_ITEM_NO )
				{
					menu_current->cursor = 0;
					menu_current->page = 0;
					screen_toggle = 0;
				}
				// Enter sub-menu
				else
				{
					menu_selection(1);
				}
			}
			// Exit sub-menu
			else
			{
				menu_current->exFlag = 0;
				menu_selection(0);
			}
		}
	}
}


/**
* @brief  Show Status display
* @param	time value in ms for time display
* @info		this function is called by RTOS in
* 				order to refresh it
**/
void display_status(uint32_t time)
{
	static uint8_t toggle = 1;
	static uint32_t time_last;

	// If Status display active
	if(screen_toggle == 0)
	{
		ssd1306_ClearD(0, 10, Black);
		screen_clock(time);
		screen_sRate();
		ssd1306_UpdateScreen();
	}

	// Main menu time, start timeout and check for end of it
	else
	{
		// Get timeout for setting display - start timeout
		if(toggle == 1)
		{
			toggle = 0;
			time_last = time;
		}
		// If settings menu overrides (sets _timeout to 0) / is active, set new time_last
		if(status_timeout == 0)
		{
			time_last = time;
		}
		// Override settings menu if timeout is reached
		// Timeout (seconds) -1 is actual timeout as this functions will loop twice, thus adding a seconds
		if( (time >= (time_last +SCREEN_TIMEOUT)) && (status_timeout == 1))
		{
			toggle = 1;
			screen_toggle = 0;
			menu_current = &g_menu; // reset back to main menu if in submenu
			ssd1306_ClearD(0, 10, Black);
			screen_clock(time);
			screen_sRate();
			ssd1306_UpdateScreen();
		}
		status_timeout = 1; // set timeout switch ready. Can be overridden by settings display action
	}
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
#define CLOCK_X		( (SSD1306_WIDTH -8*Font_11x18.FontWidth) /2)
#define CLOCK_Y		FONT_DEF.FontHeight
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
	// Convert ms to h, m, s
	h = ms /3600000;
	m = (ms -h*3600000) /60000;
	s = (ms -m*60000 -h*3600000) /1000;
	// Format display showing time
	sprintf(txt, "%02lu:%02lu:%02lu", h, m, s);

	// Print clock
	ssd1306_SetCursor(CLOCK_X, CLOCK_Y);
	ssd1306_WriteString(txt, Font_11x18, White);
}


/**
* @brief  Print current sample rate detected from usb interface
* @info		This function is used by RTOS and is called every second
**/
#define SRATE_Y		(CLOCK_Y + Font_11x18.FontHeight)
void screen_sRate(void)
{
	uint8_t SR = INTR_getSR();
	char select[10];

	if(SR >= sizeof(SamplingRates)/sizeof(SamplingRates[0]))
	{
		sprintf(select, "%s", "ERR");
	}
	else
	{
		sprintf(select, "%s", SamplingRates[SR]);
	}

	ssd1306_SetCursor(0, SRATE_Y);
	ssd1306_WriteString("SR:", Font_11x18, White);
	ssd1306_SetCursor((SSD1306_WIDTH -strlen(select) *Font_11x18.FontWidth)/2, SRATE_Y);
	ssd1306_WriteString(select, Font_11x18, White);
}
