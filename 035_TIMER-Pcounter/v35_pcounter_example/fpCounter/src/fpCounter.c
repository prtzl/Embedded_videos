#include "fpCounter.h"

// Local variables storing capture compare register values
uint32_t IC1, IC2;
uint8_t dirty_read1;

#define T0	(float)0.0000001
#define F0	10000000
#define Kt	1
#define Kf

/**
* @brief  Enables period counter input capture or CH1 and CH1
* @info		Also enables interrupt for CH1
**/
void fpCounter_p_init(void)
{
#ifndef FPCOUNTER_TIM_USE_LL
	// Enable input compare channels for Frequency and DC measurement
	HAL_TIM_IC_Start_IT(&FPCOUNTER_TIM_HANDLE, TIM_CHANNEL_1);
	HAL_TIM_IC_Start(&FPCOUNTER_TIM_HANDLE, TIM_CHANNEL_2);
#else
	LL_TIM_CC_EnableChannel(FPCOUNTER_TIM_MACRO, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_EnableChannel(FPCOUNTER_TIM_MACRO, LL_TIM_CHANNEL_CH2);
	LL_TIM_EnableIT_CC1(FPCOUNTER_TIM_MACRO);

	LL_TIM_EnableCounter(FPCOUNTER_TIM_MACRO);
#endif
}

/**
* @brief  Period counter interrupt routine
* @info		Copies input capture compare data to global variable
**/
void fpCounter_p_IRQ(void)
{
#ifndef FPCOUNTER_TIM_USE_LL
	if(__HAL_TIM_GET_FLAG(&FPCOUNTER_TIM_HANDLE, TIM_FLAG_CC1))
	{
		IC1 = __HAL_TIM_GET_COMPARE(&FPCOUNTER_TIM_HANDLE, TIM_CHANNEL_1);
		IC2 = __HAL_TIM_GET_COMPARE(&FPCOUNTER_TIM_HANDLE, TIM_CHANNEL_2);
		dirty_read1 = 1; // write complete
	}
#else
	IC1 = LL_TIM_IC_GetCaptureCH1(FPCOUNTER_TIM_MACRO);
	IC2 = LL_TIM_IC_GetCaptureCH2(FPCOUNTER_TIM_MACRO);
#endif
}

/**
* @brief  Calculates period counter data; period, frequency and duty cycle
* @param	period pointer to period variable
* @param	frequency pointer to frequency variable
* @param	duty pointer to duty variable
**/
void fpCounter_p_get(double *period, uint32_t *frequency, float *duty)
{
	// Calculate data so that IRQ does not overwrite while doing so
	do
	{
		dirty_read1 = 0;
		*period = (double)(IC1 +1) /F0;
		*frequency = (uint32_t)((float)1 / *period);
		*duty = 100 *((float)(IC2 +1))/(IC1 +1);
	}while(dirty_read1);
}

/* Demo */
/**
* @brief  Starts PWM generator
**/
void fpCounter_demo(void)
{
#ifndef FPCOUNTER_TIM_USE_LL
	HAL_TIM_PWM_Start(&FPCOUNTER_DEMO_HANDLE, FPCOUNTER_DEMO_CHANNEL);
#else
  LL_TIM_CC_EnableChannel(FPCOUNTER_DEMO_TIM_MACRO, LL_TIM_CHANNEL_CH1);
  LL_TIM_EnableCounter(FPCOUNTER_DEMO_TIM_MACRO);
#endif
}
