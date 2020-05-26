#ifndef GUI_H
#define GUI_H

#include "stm32f4xx.h"

#include "pcm1792.h"
#include "usb_interface.h"

typedef struct
{
	PCMError pcm;
	INTRError intr;
	ErrorStatus rtos;
} NewError_t;

/** Prototypes **/
void screen_init(void);

void screen_initInterface(INTRError status);
void screen_initPCM(PCMError status);
void screen_initRTOS(ErrorStatus status);
void screen_initStatus(NewError_t *status);

void screen_startDAC(void);
void screen_clock(uint32_t ms_in);


#endif
