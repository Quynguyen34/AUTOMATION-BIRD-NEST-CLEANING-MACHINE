/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
/* USER CODE BEGIN Variables */
osThreadId startMainSystemHandle;
osThreadId startUART_RXHandle;
osThreadId startUART_TXHandle;
osThreadId startGRABHandle;
/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void StartMainSystem(void const * argument);
void StartUART_RX(void const * argument);
void StartUART_TX(void const * argument);
void StartGRAB(void const * argument);
/* USER CODE END FunctionPrototypes */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* Private application code --------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN Variables */
#define max(a,b) (a > b) ? a : b;
#define min(a,b) (a > b) ? b : a;
/* USER CODE END Variables */
/* USER CODE BEGIN FunctionPrototypes */
/*----------------------------------*/
/* Config speed for stepper */
int max3(int a, int b, int c) {
	int n1 = max(a, b);
    return max(n1, c);
}
/* Function set home */
void DelayUs_step(uint32_t us)
{
	HAL_TIM_Base_Start_IT(&htim1);
	//(&htim7)->Instance->CNT = (0);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while(__HAL_TIM_GET_COUNTER(&htim1) < us);
	HAL_TIM_Base_Stop_IT(&htim1);
}
void HOME(void)
{
    // Home Z axis
    HAL_GPIO_WritePin(dir_3_port, dir_3_pin, GPIO_PIN_RESET); // Set direction to move towards home
    while (HAL_GPIO_ReadPin(moveZsub_port, moveZsub_pin) != CNC_pos.Lsw6) {
        HAL_GPIO_TogglePin(step_3_port, step_3_pin); // Toggle step pin to move towards home
        DelayUs_step(30); // Adjust delay as needed
    }
    // Home X axis
    HAL_GPIO_WritePin(step_1_port, dir_1_pin, GPIO_PIN_RESET); // Set direction to move towards home
    bool isXHome = false;

    // Home Y axis
    HAL_GPIO_WritePin(dir_2_port, dir_2_pin, GPIO_PIN_RESET); // Set direction to move towards home
    bool isYHome = false;

    while (!(isXHome && isYHome)) {
        if (!isXHome && (HAL_GPIO_ReadPin(moveXsub_port, moveXsub_pin) != CNC_pos.Lsw2)) {
            HAL_GPIO_TogglePin(step_1_port, step_1_pin); // Toggle step pin to move towards home
        } else {
            isXHome = true;
        }

        if (!isYHome && (HAL_GPIO_ReadPin(moveYsub_port, moveYsub_pin) != CNC_pos.Lsw4)) {
            HAL_GPIO_TogglePin(step_2_port, step_2_pin); // Toggle step pin to move towards home
        } else {
            isYHome = true;
        }
        DelayUs_step(30); // Adjust delay as needed
    }
}

/* Function control 3 axis */
//Move X-Y
void MoveToPosXY(float x, float y) {
    trans_to_posXY(x,y);
    CNC_pos.MoveX = caculate_pos(CNC.set_posX, 161);
    CNC_pos.MoveY = caculate_pos(CNC.set_posY, 161);
    int step_max = max3(abs(CNC_pos.MoveX), abs(CNC_pos.MoveY), abs(CNC_pos.MoveZ));
    float coef1 = fabs(CNC_pos.MoveX) / step_max;
    float coef2 = fabs(CNC_pos.MoveY) / step_max;
    CNC_pos.pos1dot = CNC_pos.max_speedXY * coef1;
    CNC_pos.pos2dot = CNC_pos.max_speedXY * coef2;
    CNC_pos.accel1 = CNC_pos.a_maxX * coef1;
    CNC_pos.accel2 = CNC_pos.a_maxY * coef2;
    CNC_pos.jerk1 = CNC_pos.j_maxX * coef1;
    CNC_pos.jerk2 = CNC_pos.j_maxY * coef2;
    if (Stepper1.run_state != 1 && Stepper2.run_state != 1) {
        Accel_Stepper_Move(&Stepper1, CNC_pos.MoveX, CNC_pos.accel1, CNC_pos.jerk1, CNC_pos.pos1dot);
        Accel_Stepper_Move(&Stepper2, CNC_pos.MoveY, CNC_pos.accel2, CNC_pos.jerk2, CNC_pos.pos2dot);
        while (Stepper1.run_state != STOP || Stepper2.run_state != STOP) {
        	if(HAL_GPIO_ReadPin(moveXplus_port, moveXplus_pin) == CNC_pos.Lsw1 ||
        		HAL_GPIO_ReadPin(moveYplus_port, moveYplus_pin) == CNC_pos.Lsw3){
        		//handle_stop_button_press();
        	}
        	osDelay(1);
        }
        CNC.pos_x = x;
        CNC.pos_y = y;
    }
}
// Move Z
void MoveToPosZ(float z) {
    trans_to_posZ(z);
    CNC_pos.MoveZ = caculate_pos(CNC.set_posZ, 161);
    int step_max = max3(abs(CNC_pos.MoveX), abs(CNC_pos.MoveY), abs(CNC_pos.MoveZ));
    float coef3 = fabs(CNC_pos.MoveZ) / step_max;
    CNC_pos.pos3dot = CNC_pos.max_speedZ * coef3;
    CNC_pos.accel3 = CNC_pos.a_maxZ * coef3;
    CNC_pos.jerk3 = CNC_pos.j_maxZ * coef3;
    if (Stepper3.run_state != 1) {
        Accel_Stepper_Move(&Stepper3, CNC_pos.MoveZ, CNC_pos.accel3, CNC_pos.jerk3, CNC_pos.pos3dot);
        while(Stepper3.run_state != STOP){
        	if(HAL_GPIO_ReadPin(moveZplus_port, moveZplus_pin) ==  CNC_pos.Lsw5){
        		//handle_stop_button_press();
        	}
			osDelay(1);
		}
        CNC.pos_z = z;
    }
}


/* USER CODE BEGIN Application */
void MX_FREERTOS_Unit(void)
{
	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* definition and creation of startMainSystem */
	osThreadDef(startMainSystem, StartMainSystem, osPriorityNormal, 0, 128);
	startMainSystemHandle = osThreadCreate(osThread(startMainSystem), NULL);

	/* definition and creation of startUART_RX */
	osThreadDef(startUART_RX, StartUART_RX, osPriorityLow, 0, 128);
	startUART_RXHandle = osThreadCreate(osThread(startUART_RX), NULL);

	/* definition and creation of startUART_TX */
	osThreadDef(startUART_TX, StartUART_TX, osPriorityLow, 0, 128);
	startUART_TXHandle = osThreadCreate(osThread(startUART_TX), NULL);

	/* definition and creation of startGRAB */
	osThreadDef(startGRAB, StartGRAB, osPriorityLow, 0, 128);
	startGRABHandle = osThreadCreate(osThread(startGRAB), NULL);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartMainSystem */
/**
  * @brief  Function implementing the startMainSystem thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartMainSystem */

void StartMainSystem(void const * argument)
{
  /* USER CODE BEGIN 5 */
  HOME();
  initializeCNC_pos(&CNC_pos);
  Accel_Stepper_SetPin(&Stepper1, step_1_port, step_1_pin, dir_1_port, dir_1_pin);
  Accel_Stepper_SetPin(&Stepper2, step_2_port, step_2_pin, dir_2_port, dir_2_pin);
  Accel_Stepper_SetPin(&Stepper3, step_3_port, step_3_pin, dir_3_port, dir_3_pin);

  Accel_Stepper_SetTimer(&Stepper1, &htim2);
  Accel_Stepper_SetTimer(&Stepper2, &htim3);
  Accel_Stepper_SetTimer(&Stepper3, &htim4);

  CNC.pos_x = 0;
  CNC.pos_y = 0;
  CNC.pos_z = 0;
  /* Infinite loop */
  for(;;)
  {
//	if (state.start_press) {
//	  HAL_GPIO_WritePin(drill_port, drill_pin, 1);
//	  move_to_coordinates();
//	}
	osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartUART_RX */
/**
* @brief Function implementing the startUART_RX thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUART_RX */
void StartUART_RX(void const * argument)
{
  /* USER CODE BEGIN StartUART_RX */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartUART_RX */
}

/* USER CODE BEGIN Header_StartUART_TX */
/**
* @brief Function implementing the startUART_TX thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUART_TX */
void StartUART_TX(void const * argument)
{
  /* USER CODE BEGIN StartUART_TX */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartUART_TX */
}

/* USER CODE BEGIN Header_StartGRAB */
/**
* @brief Function implementing the startGRAB thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGRAB */
void StartGRAB(void const * argument)
{
  /* USER CODE BEGIN StartGRAB */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartGRAB */
}
/* USER CODE END Application */

