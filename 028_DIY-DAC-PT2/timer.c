#include "timer.h"

/** Delay in ms **/
void delay_ms(uint32_t ms)
{
	uint32_t cnt = timer_getms() + ms;

	while( timer_getms() < cnt );
}
