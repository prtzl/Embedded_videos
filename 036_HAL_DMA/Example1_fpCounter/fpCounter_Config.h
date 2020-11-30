/* Data handling - choose one*/
// Uncomment below to use DMA for data transfer
#define FPCOUNTER_USE_DMA
// If you use LL driver, populate following macros
#define FPCOUNTER_DMA		DMA1
#define FPCOUNTER_DMA_CH1	LL_DMA_STREAM_0
#define FPCOUNTER_DMA_CH2	LL_DMA_STREAM_3

// Uncomment below to use ISR for data transfer
//#define FPCOUNTER_USE_ISR

/* Configuration: define handles for counter timers */
#define FPCOUNTER_TIM_HANDLE	htim4
#define FPCOUNTER_TIM_MACRO		TIM4

// Un-comment line below to enable demo timer functions
#define FPCOUNTER_DEMO_HANDLE		htim5
#define FPCOUNTER_DEMO_TIM_MACRO	TIM5
#define FPCOUNTER_DEMO_CHANNEL		TIM_CHANNEL_1 // change to yours
#define FPCOUNTER_DEMO_CHANNEL_LL	LL_TIM_CHANNEL_CH1 // change to yours
