#include "fpCounter.h"

// Local variables storing capture compare register values
static uint32_t IC1, IC2;

#define T0	0.0000001f
#define F0	10000000U
#define Kt	1U
#define Kf

/**
* @brief  Enables period counter input capture or CH1 and CH1
* @info		Also enables interrupts or DMA for data transfer
**/
HAL_StatusTypeDef fpCounter_p_init(void)
{
	HAL_StatusTypeDef ret = HAL_OK; // status
#ifndef FPCOUNTER_TIM_USE_LL
#ifdef FPCOUNTER_USE_ISR
	// Enable input compare channels for Frequency and DC measurement
	ret = HAL_TIM_IC_Start_IT(&FPCOUNTER_TIM_HANDLE, TIM_CHANNEL_1);
	if(ret != HAL_OK)
	{
		return ret;
	}
	ret = HAL_TIM_IC_Start(&FPCOUNTER_TIM_HANDLE, TIM_CHANNEL_2);
	if(ret != HAL_OK)
	{
		return ret;
	}
#endif
#ifdef FPCOUNTER_USE_DMA
	ret = HAL_TIM_IC_Start_DMA(&FPCOUNTER_TIM_HANDLE, TIM_CHANNEL_1, &IC1, 1);
	if(ret != HAL_OK)
	{
		return ret;
	}
	FPCOUNTER_TIM_HANDLE.State = HAL_TIM_STATE_READY; // funny HAL thing (or me)
	ret = HAL_TIM_IC_Start_DMA(&FPCOUNTER_TIM_HANDLE, TIM_CHANNEL_2, &IC2, 1);
	if(ret != HAL_OK)
	{
		return ret;
	}
#endif
#else
	LL_TIM_CC_EnableChannel(FPCOUNTER_TIM_MACRO, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_EnableChannel(FPCOUNTER_TIM_MACRO, LL_TIM_CHANNEL_CH2);
#ifdef FPCOUNTER_USE_ISR
	LL_TIM_EnableIT_CC1(FPCOUNTER_TIM_MACRO);
#endif
#ifdef FPCOUNTER_USE_DMA
	// CH1
	// Set up dma for IC1
	LL_DMA_SetDataLength(FPCOUNTER_DMA, FPCOUNTER_DMA_CH1, 1);
	LL_DMA_SetMemoryAddress(FPCOUNTER_DMA, FPCOUNTER_DMA_CH1, (uint32_t)&IC1);
	LL_DMA_SetPeriphAddress(FPCOUNTER_DMA, FPCOUNTER_DMA_CH1, (uint32_t)&FPCOUNTER_TIM_MACRO->CCR1);
	LL_DMA_EnableStream(FPCOUNTER_DMA, FPCOUNTER_DMA_CH1);
	// set tim request for IC1
	LL_TIM_EnableDMAReq_CC1(FPCOUNTER_TIM_MACRO);
	// CH2
	// Set up dma for IC2
	LL_DMA_SetDataLength(FPCOUNTER_DMA, FPCOUNTER_DMA_CH2, 1);
	LL_DMA_SetMemoryAddress(FPCOUNTER_DMA, FPCOUNTER_DMA_CH2, (uint32_t)&IC2);
	LL_DMA_SetPeriphAddress(FPCOUNTER_DMA, FPCOUNTER_DMA_CH2, (uint32_t)&FPCOUNTER_TIM_MACRO->CCR2);
	LL_DMA_EnableStream(FPCOUNTER_DMA, FPCOUNTER_DMA_CH2);
	// set tim request for IC2
	LL_TIM_EnableDMAReq_CC2(FPCOUNTER_TIM_MACRO);
#endif
	LL_TIM_EnableCounter(FPCOUNTER_TIM_MACRO);
#endif
	return HAL_OK;
}

/**
* @brief  Period counter interrupt routine
* @info		Copies input capture compare data to global variable
**/
void fpCounter_p_IRQ(void)
{
#ifdef FPCOUNTER_USE_ISR
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
	uint32_t ic1 = IC1, ic2 = IC2;

	*period = (double)(ic1 +1) /F0;
	*frequency = (uint32_t)((float)1 / *period);
	*duty = 100 *((float)(ic2 +1))/(ic1 +1);
}

/* Demo */
/**
* @brief  Starts PWM generator
**/
HAL_StatusTypeDef fpCounter_demo(void)
{
	HAL_StatusTypeDef ret = HAL_OK;
#ifndef FPCOUNTER_TIM_USE_LL
	ret = HAL_TIM_PWM_Start(&FPCOUNTER_DEMO_HANDLE, FPCOUNTER_DEMO_CHANNEL);
	if(ret != HAL_OK)
	{
		return ret;
	}
#else
  LL_TIM_CC_EnableChannel(FPCOUNTER_DEMO_TIM_MACRO, LL_TIM_CHANNEL_CH1);
  LL_TIM_EnableCounter(FPCOUNTER_DEMO_TIM_MACRO);
#endif
  return HAL_OK;
}
