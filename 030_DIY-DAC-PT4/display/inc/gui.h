#ifndef GUI_H
#define GUI_H

#include "stm32f4xx.h"
#include "pcm1792.h"
#include "usb_interface.h"

// defined when testing conditions are needed
#define TESTING
#define FONT_DEF						Font_7x10

#define MENU_ITEM_NO				6+1 // how many you want + exit
#define SUBMENU_ITEM_NO			6+1

#define MENU_NAME_LEN				(128 /7 -1 +1) // screenwidth/charwidth -cursor +0terminator
#define SUBMENU_NAME_LEN		(128 /7 -1 +1)

// Create function pointer typedef
typedef void(*pFunction)(void);

// Error structure
typedef struct
{
	PCMError pcm;
	INTRError intr;
	ErrorStatus rtos;
} NewError_t;

// Menu item item, holds menu item information
typedef struct
{
	char name[MENU_NAME_LEN];				// menu item name
	char options[SUBMENU_ITEM_NO][SUBMENU_NAME_LEN];	// sub menu item names
	uint8_t noitems;		// update according to size of options
	pFunction actions[SUBMENU_ITEM_NO];
} MenuItem_t;

// Menu / sub-menu structure item, hold menu information
typedef struct
{
	uint8_t exFlag;
	uint8_t cursor;					// cursor location -> good as it will remember last position/selection
	uint8_t exCursor;				// cursor position before exit
	uint8_t page;						// page location -||-
	uint8_t exPage;					// page position before exit
	uint8_t cursor_limUP;		// cursor limit up
	uint8_t cursor_limDOWN;	// cursor limit down
	uint8_t page_size;			// size of menu page, number of rows
	uint8_t nopages;				// number of all pages
	uint8_t issubmen;				// is this a sub menu
	uint8_t submenpos;			// sub menu position -> to which menu does it belong
} MENU_t;

/** Prototypes **/
void screen_init(void);

void screen_initInterface(INTRError status);
void screen_initPCM(PCMError status);
void screen_initRTOS(ErrorStatus status);
void screen_initStatus(NewError_t *status);

void screen_start(void);
void screen_clock(uint32_t ms_in);
void screen_sRate(void);

void move_cursor_up(MENU_t *menu);
void move_cursor_down(MENU_t *menu);
void move_page_up( MENU_t *menu);
void move_page_down(MENU_t *menu);
void menu_selection(uint8_t submen);

void display_settings(uint8_t up, uint8_t down, uint8_t ok);
void display_status(uint32_t time);

#endif
