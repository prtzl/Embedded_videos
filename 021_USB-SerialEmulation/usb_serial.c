/*
 Include this pair of usb_serial.c and usb_serial.h files in your project.
 No need to add anything to the code, like in the vides. All includes
 are already in the header file. Call this function to send strings
 over usb serial.
*/

#include "usb_serial.h"

void usb_sendSerial_string(uint8_t * data)
{
	uint8_t ret;

  	ret = CDC_Transmit_FS(data, sizeof(data));
  	if(ret != HAL_OK)
  	{
  		Error_Handler();
  	}
}