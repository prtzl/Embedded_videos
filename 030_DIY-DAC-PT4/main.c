/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "swo.h"

#include "pcm1792.h"
#include "rtos.h"
#include "gui.h"
#include "ssd1306.h"
#include "buttons.h"
#include "timer.h"

#include "seq.h"
#include "single.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// Handle for PCM
extern PCM1792_TypeDef PCM1792;
extern SSD1306_t SSD1306;
extern uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
extern Button_t Buttons[BUTTONS_NO];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_SPI3_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */

  // Structure of device errors
  NewError_t err_list = {ErrorPcm, ErrorIntrPwr, ERROR};

  /** System time init **/
  time_init();

  /** Buttons init **/
  buttonInit();

  /** SSD1306 display init **/
  // Set contrast of display 0 - 255 : dark - bright
  ssd1306_Init(0xff); //0x3f

  // Print cute startup animation
#ifdef TESTING
  uint32_t i, j;
  for(i = 0; i < sizeof(seq)/sizeof(seq[0]); i++)
  {
  	for(j = SSD1306_WIDTH; j < sizeof(seq[0])/sizeof(seq[0][0]); j++)
  	{
  		SSD1306_Buffer[j] = seq[i][j];
  	}
  	ssd1306_UpdateScreen();
  	delay_ms(30);
  }
#endif
  // Print start display
  screen_init();

  /** USB AUDIO INTERFACE init **/
  err_list.intr = INTR_Init();
  if( err_list.intr != INTROK )
  {
  	Error_Handler();
  }

  /** PCM1792 init **/
  err_list.pcm = PCM1792_Init(&PCM1792);
  if( err_list.pcm != PCMOK )
  {
  	Error_Handler();
  }

  /** RTOS init **/
  err_list.rtos = rtos_init(20000);
  if( err_list.rtos != SUCCESS )
  {
  	Error_Handler();
  }

  /** DISPLAY INIT INFO **/
  screen_initStatus(&err_list);

  /** Check for errors to stop **/
#ifndef TESTING
  if( (err_list.intr | err_list.pcm | err_list.rtos) != 0 )
  {
  	while(1);
  }
#endif

  /** Start operation **/
  PCM1792_Start(&PCM1792); // start dac
  screen_start();
  rtos_enable(); // enable rtos

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	// Button shit
	uint8_t up, down, ok;
	uint8_t up_last = 0, down_last = 0, ok_last = 0;
	uint8_t up_action = 0, down_action = 0, ok_action = 0;
  while (1)
  {

  	// Read buttons
  	up = buttonRead(BUP);
  	ok = buttonRead(BOK);
  	down = buttonRead(BDOWN);

  	// Latch button press
  	if( up != up_last )	if( up == 1 ) up_action = 1;
  	up_last = up;
  	if( down != down_last )	if( down == 1 ) down_action = 1;
  	down_last = down;
  	if( ok != ok_last )	if( ok == 1 ) ok_action = 1;
  	ok_last = ok;

  	// Action on key presses
  	if( up_action | down_action | ok_action )
  	{
  		display_settings(up_action, down_action, ok_action);
  		up_action = 0;
  		down_action = 0;
  		ok_action = 0;
  	}

		LL_GPIO_TogglePin(GPIOD, LL_GPIO_PIN_13);
//		delay_ms(1000);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_5)
  {
  Error_Handler();  
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
    
  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_4, 168, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_4);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_Init1msTick(168000000);
  LL_SetSystemCoreClock(168000000);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
