#include "usb_interface.h"
#include "timer.h"

/**
* @brief  Init USB interface
* @info		Check if USB interface is connected and working
* 				before starting DAC operation
* @retval INTRError
**/
INTRError INTR_Init(void)
{
	uint32_t timeout;

	// Check POWER
	timeout  = timer_getms();
	while( !LL_GPIO_IsInputPinSet(INTR_PWR_PORT, INTR_PWR_PIN) )
	{
		if( timer_getms() >= (timeout+10) )
		{
			return ErrorIntrPwr;
		}
	}

	// CHECK USB CONNECTED SIGNAL?
//	timeout  = timer_getms();
//	while( !LL_GPIO_IsInputPinSet(INTR_USB_PORT, INTR_USB_PIN) )
//	{
//		if( timer_getms() >= (timeout+10) )
//		{
//			return ErrorIntrUsb;
//		}
//	}

	return INTROK;
}

uint8_t INTR_getSR(void)
{
	uint8_t SR = 0;

	SR = LL_GPIO_IsInputPinSet(INTR_SR0_PORT, INTR_SR0_PIN);
	SR += LL_GPIO_IsInputPinSet(INTR_SR1_PORT, INTR_SR1_PIN) *(uint8_t)2;
	SR += LL_GPIO_IsInputPinSet(INTR_SR2_PORT, INTR_SR2_PIN) *(uint8_t)4;
	SR += LL_GPIO_IsInputPinSet(INTR_SR3_PORT, INTR_SR3_PIN) *(uint8_t)8;

	return SR;
}
