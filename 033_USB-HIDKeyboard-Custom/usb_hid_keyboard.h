#ifndef __USB_HID_KEYBOARD_H
#define __USB_HID_KEYBOARD_H

#include "main.h"

// un-comment line below if you are using HID example, recommended using custom HID
#define USE_CUSTOM_HID
#if defined(USE_CUSTOM_HID)
#include "usbd_custom_hid_if.h"
#else
#include "usbd_hid.h"
#endif

// USB keyboard codes
#define USB_HID_MODIFIER_LEFT_CTRL   0x01
#define USB_HID_MODIFIER_LEFT_SHIFT  0x02
#define USB_HID_MODIFIER_LEFT_ALT    0x04
#define USB_HID_MODIFIER_LEFT_GUI    0x08 // (Win/Apple/Meta)
#define USB_HID_MODIFIER_RIGHT_CTRL  0x10
#define USB_HID_MODIFIER_RIGHT_SHIFT 0x20
#define USB_HID_MODIFIER_RIGHT_ALT   0x40
#define USB_HID_MODIFIER_RIGHT_GUI   0x80

#define USB_HID_SPACE		0x2C
#define USB_HID_DOT			0x37
#define USB_HID_NEWLINE	0x28
#define USB_HID_FSLASH	0x38
#define USB_HID_BSLASH	0x31
#define USB_HID_COMMA		0x36

void USB_Keyboard_SendChar(char ch);
void USB_Keyboard_SendString(char * s);

#endif
