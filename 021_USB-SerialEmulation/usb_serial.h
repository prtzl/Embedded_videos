#ifndef __USBSERIAL_H
#define __USBSERIAL_H

#include "main.h"
#include "usbd_cdc_if.h"

void usb_sendSerial_string(uint8_t * data);

#endif