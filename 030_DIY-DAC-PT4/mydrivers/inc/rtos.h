#ifndef RTOS_H
#define RTOS_H

#include "stm32f4xx.h"

#define USE_RTOS_TIMER

ErrorStatus rtos_init(uint32_t slice_us);
void rtos_enable(void);
void rtos_disable(void);

#endif
