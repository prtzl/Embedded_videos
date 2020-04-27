#include "pcm1792.h"
#include "spi_my.h"
#include "timer.h"

extern PCM1792_TypeDef PCM1792;

int main(void)
{

  /** Init PCM1792 **/
  if( PCM1792_Init(&PCM1792) != SUCCESS )
  {
  	LL_GPIO_WriteOutputPort(GPIOD, LL_GPIO_PIN_12 |LL_GPIO_PIN_13 |LL_GPIO_PIN_14 |LL_GPIO_PIN_15);
  	while(1);
  }

  // Enable new system timer
  LL_TIM_SetCounter(TIM5, 0);
  LL_TIM_EnableCounter(TIM5);

  // We will toggle this
  uint8_t mute = 1;

  while (1)
  {

  	PCM1792_MuteCtrl(&PCM1792, mute);
  	mute = !mute;

  	LL_GPIO_TogglePin(GPIOD, LL_GPIO_PIN_13);
  	delay_ms(1000);

  }
}