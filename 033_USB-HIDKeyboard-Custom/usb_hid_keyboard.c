/*
 * This code now supports HID and Custom HID library examples;
 * difference is in library included in header file and
 * a few function calls for Sending Reports in "USB_Keyboard_SendChar" function
 *
 *******************************************************************************
 * For HID:
 * -replace Mouse report descriptor for keyboard
 *  report descriptor in "usbd_hid.h" and change report descriptor
 *  size in "usbd_conf.h"
 * -change device descriptor for FS and HS to 0x01 for
 *	keyboard (default 0x02 for mouse)
 *
 *	Note: every time you change and save your CubeMx configuration, library
 *	settings get reset, so Device and Report descriptors and its sizes will
 *	be reset and you will need to change them back -> recommend using Custom HID
 ******************************************************************************
 * For Custom HID:
 * -add keyboard report descriptor provided in repository or from
 *  USB HID documentation (v. 1.11) page 79 and copy contents into
 * 	"uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE]"
 * 	variable in file "usbd_custom_hid.c" This way, report descriptor
 * 	won't be overwritten every time you change something within cube;
 * 	make sure you print it between "USER CODE" comments; 0xC0 is already there
 *
 * -Other settings to change in CubeMx under USB_DEVICE setting:
 * 	-change USBD_CUSTOM_HID_REPORT_DESC_SIZE to "63" using usb keyboard example
 * 	-change CUSTOM_HID_FS_BINTERVAL to 0x0A as in example
 * 	-change USBD_CUSTOMHID_OUTREPORT_BUF_SIZE to 8
 *
 * Now your keyboard / custom HID setting will not be changed when modifying
 * your cube configuration, therefore my recommended choice for your
 * mouse/keyboard/controller or custom device implementation is "custom HID"
 *
 ******************************************************************************
 * Include this code in your project.
 * You can also add this and the header file to your project.
 * Make sure to include header file in main.c
 * Some actions are based on English keyboard layout.
 * Only letters and symbols '.', "space", "enter" '!' and '?' are implemented.
 * Add cases in switch statement at line 38 for additional symbols and actions
 *
 * NOTE:
 * key combinations such as ?,!,@ are created as you would on QWERTY US layout
*/

#include "usb_hid_keyboard.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
static uint8_t HID_buffer[8];

static void USB_charToCode(char ch)
{
	// Check if lower or upper case
	if(ch >= 'a' && ch <= 'z')
	{
		HID_buffer[0] = 0;
		// convert ch to HID letter, starting at a = 4
		HID_buffer[2] = (uint8_t)(4 + (ch - 'a'));
	}
	else if(ch >= 'A' && ch <= 'Z')
	{
		// Add left shift
		HID_buffer[0] = USB_HID_MODIFIER_LEFT_SHIFT;
		// convert ch to lower case
		ch = ch - ('A'-'a');
		// convert ch to HID letter, starting at a = 4
		HID_buffer[2] = (uint8_t)(4 + (ch - 'a'));
	}
	else if(ch >= '0' && ch <= '9') // Check if number
	{
		HID_buffer[0] = 0;
		// convert ch to HID number, starting at 1 = 30, 0 = 39
		if(ch == '0')
		{
			HID_buffer[2] = 39;
		}
		else
		{
			HID_buffer[2] = (uint8_t)(30 + (ch - '1'));
		}
	}
	else // not a letter nor a number
	{
		switch(ch)
		{
			case ' ':
				HID_buffer[0] = 0;
				HID_buffer[2] = 44;
				break;
			case '.':
				HID_buffer[0] = 0;
				HID_buffer[2] = 55;
				break;
			case '\n':
				HID_buffer[0] = 0;
				HID_buffer[2] = 40;
				break;
			case '!':
				//combination of shift modifier and key
				HID_buffer[0] = USB_HID_MODIFIER_LEFT_SHIFT;	// shift
				HID_buffer[2] = 30; // number 1
				break;
			case '?':
				//combination of shift modifier and key
				HID_buffer[0] = USB_HID_MODIFIER_LEFT_SHIFT;	// shift
				HID_buffer[2] = USB_HID_FSLASH; // key '/'
				break;
			case '@':
				//combination of shift modifier and key
				HID_buffer[0] = USB_HID_MODIFIER_LEFT_SHIFT;	// shift
				HID_buffer[2] = 31; // number 2
				break;
			default:
				HID_buffer[0] = 0;
				HID_buffer[2] = 0; // not implemented
		}
	}
}

// Send character as a single key press
void USB_Keyboard_SendChar(char ch)
{
	uint8_t ret;

	// Convert character to usb hid key code
	USB_charToCode(ch);

	// press keys
#if defined(USE_CUSTOM_HID)
	ret = USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, HID_buffer, 8);
#else
	ret = USBD_HID_SendReport(&hUsbDeviceFS, HID_buffer, 8);
#endif
	if(ret != HAL_OK)
	{
		Error_Handler();
	}
	HAL_Delay(15);

	// release keys
	HID_buffer[0] = 0;
	HID_buffer[2] = 0;
#if defined(USE_CUSTOM_HID)
#else
	ret = USBD_HID_SendReport(&hUsbDeviceFS, HID_buffer, 8);
#endif
	ret = USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, HID_buffer, 8);
	if(ret != HAL_OK)
	{
		Error_Handler();
	}
	HAL_Delay(15);
}

// Send string as letters
void USB_Keyboard_SendString(char * s)
{
	uint8_t i = 0;

	while(*(s+i))
	{
		USB_Keyboard_SendChar(*(s+i));
		i++;
	}
}
