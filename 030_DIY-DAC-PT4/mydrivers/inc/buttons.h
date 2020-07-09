#ifndef BUTTONS_H
#define BUTTONS_H

#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"

/*
 * Buttons are defined with macros instead of arrays
 * When dealing with lower amount of buttons / items
 * this isn't a problem
 */
// number of buttons
#define BUTTONS_NO 					3

#define BUTTON_GPIO_Port		GPIOA
#define BUTTON_Pin					LL_GPIO_PIN_0

#define BUTTON_UP_Port			GPIOC
#define BUTTON_UP_Pin				LL_GPIO_PIN_6

#define BUTTON_DOWN_Port		GPIOC
#define BUTTON_DOWN_Pin			LL_GPIO_PIN_7

#define BUTTON_OK_Port			GPIOC
#define BUTTON_OK_Pin				LL_GPIO_PIN_8

typedef enum
{
	OPEN,
	CLOSED
} BtnStatus_t;

typedef enum
{
	BUP,
	BOK,
	BDOWN
} BtnPos;

typedef struct
{
	uint32_t pin;
	GPIO_TypeDef *port;
	uint8_t state;
	uint8_t debounce;
	BtnStatus_t status;
} Button_t;

void buttonInit(void);
uint8_t buttonRead(BtnPos i);

#endif
