/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


#include "rtos.h"
#include "rtos_tasks.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

extern uint32_t rtos_slice_us;
// There is external defined array of pointers of type rtos_task_t
extern rtos_task_t *rtos_task_list[];

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /** Scheduler **/
  /**
    NOT CYCLIC ANYMORE
    ALLWAYS go over whole list and look
    which task is ready to execute

    And by selecting task priority, now by checking
    whick task has waited the longest, we break our initial rule
    that each task gets its determened time slice
    We could also look at task priority instead of lastTick

    Also, task periods should be multiples of time slice for better operations
  **/

  // Log rtos ticks -> every time this functions is called
  static uint32_t rtos_tick = 0;
  rtos_tick++;
  uint8_t i = 0; // task counter
  rtos_task_t *currTask = 0; // pointer to current task

  /**
   * Too long task? If there is another task pending (should not happen)
   **/
  if( SCB->ICSR & SCB_ICSR_PENDSTSET_Msk )
  {
    while(1); /** or alert of this problem **/
  }

  /**
   * Assign current task in list to be executed only if time passed in rtos_ticks
   * from previous time this task has been executed is at least the task period
   * so we look at task last_tick and its period_ms
   * ( task sets its own period  of execution relative to rtos_tick)
   *
   * (currentRtosTick - lastTaskTick) >= taskPeriod -> assign task to be executed
   *
   * But first check priority
   * So only execute new task if its priority is larger then before task OR if priority is equal,
   * then sort by which one has waited the longest
   * Therefore lower priority tasks will be executed only if no other tasks have been selected before
   *
   * Keep in mind, that if task with biggest priority has a large frequency of execution, other tasks
   * wont execute at all
  **/
  // GO over all tasks until 0
  while(rtos_task_list[i] != 0)
  {
    // Append new task if is its time
    if( (rtos_tick - rtos_task_list[i]->last_tick)*rtos_slice_us/1000 >= rtos_task_list[i]->period_ms )
    {
      // Only append task if not pointing to any other task
      if(currTask == 0)
      {
        currTask = rtos_task_list[i]; // Assign task
      }
      // We have another task appended
      else
      {
        // Why should you assign new task? Biggest delay? Priority?
        // We will make decision based on which task has waited the longest: newTask->last_tick < currentTask->lastTick

        // But first we can handle important tasks by looking at different priority
        if( rtos_task_list[i]->priority < currTask->priority )
        {
          currTask = rtos_task_list[i]; // Assign task
        }

        // If priority is not greater but equal
        else if( rtos_task_list[i]->priority == currTask->priority )
        {
          // Now check which tasks has waited the longest
          if(rtos_task_list[i]->last_tick < currTask->last_tick)
          {
            currTask = rtos_task_list[i]; //assign new task
          }
        }
        // If priority of task is smaller, then don't execute it then
      }
    }
    // increment task counter regardless
    i++;
  }

  // Execute task if any has been assigned
  if(currTask != 0)
  {
    currTask->task_function();
    currTask->last_tick = rtos_tick;
  }

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
