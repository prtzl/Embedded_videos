#ifndef __USB_HID_KEYBOARD_H
#define __USB_HID_KEYBOARD_H

#include "usbd_hid.h"

void USB_Keyboard_SendChar(char ch);
void USB_Keyboard_SendString(char * s);

#endif
