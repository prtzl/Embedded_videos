#ifndef USB_INTERFACE_H
#define USB_INTERFACE_H

#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"

#define INTR_PWR_PORT		GPIOA
#define INTR_PWR_PIN		LL_GPIO_PIN_1

#define INTR_USB_PORT		GPIOA
#define INTR_USB_PIN		LL_GPIO_PIN_3

typedef enum
{
	INTROK,
	ErrorIntrPwr,
	ErrorIntrUsb
} INTRError;

INTRError INTR_Init(void);

#endif
