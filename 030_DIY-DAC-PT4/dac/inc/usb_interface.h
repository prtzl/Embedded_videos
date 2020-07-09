#ifndef USB_INTERFACE_H
#define USB_INTERFACE_H

#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"

#define INTR_PWR_PORT		GPIOA
#define INTR_PWR_PIN		LL_GPIO_PIN_1

#define INTR_USB_PORT		GPIOA
#define INTR_USB_PIN		LL_GPIO_PIN_3

#define INTR_SR0_PORT		GPIOE
#define INTR_SR0_PIN		LL_GPIO_PIN_7

#define INTR_SR1_PORT		GPIOE
#define INTR_SR1_PIN		LL_GPIO_PIN_8

#define INTR_SR2_PORT		GPIOE
#define INTR_SR2_PIN		LL_GPIO_PIN_9

#define INTR_SR3_PORT		GPIOE
#define INTR_SR3_PIN		LL_GPIO_PIN_10

typedef enum
{
	INTROK,
	ErrorIntrPwr,
	ErrorIntrUsb
} INTRError;

// Sampling rate modes
typedef enum
{
	SR_32k,
	SR_44k1,
	SR_48k,
	SR_88k2,
	SR_96k,
	SR_176k4,
	SR_192k,
	SR_352k8,
	SR_384k
} SamplingRate_t;

static const char SamplingRates[][10] =
{
		{"32k"},
		{"44k1"},
		{"48k"},
		{"88k2"},
		{"96k"},
		{"176k4"},
		{"192k"},
		{"352k8"},
		{"384k"}
};

INTRError INTR_Init(void);
uint8_t INTR_getSR(void);

#endif
