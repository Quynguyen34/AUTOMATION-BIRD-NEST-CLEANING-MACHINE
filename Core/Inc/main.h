/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdint.h"
#include "math.h"
#include "gpio.h"
#include "tim.h"
#include "uart.h"
#include "MoveToPos.h"
#include "Accelstepper.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
// Step pin - dir pin
#define step_1_pin GPIO_PIN_0
#define step_1_port GPIOA
#define dir_1_pin GPIO_PIN_1
#define dir_1_port GPIOA
#define step_2_pin GPIO_PIN_2
#define step_2_port GPIOA
#define dir_2_pin GPIO_PIN_3
#define dir_2_port GPIOA
#define step_3_pin GPIO_PIN_4
#define step_3_port GPIOA
#define dir_3_pin GPIO_PIN_5
#define dir_3_port GPIOA
// Switch pin
#define moveXplus_pin GPIO_PIN_6
#define moveXplus_port GPIOA
#define moveXsub_pin GPIO_PIN_7
#define moveXsub_port GPIOA
#define moveYplus_pin GPIO_PIN_0
#define moveYplus_port GPIOB
#define moveYsub_pin GPIO_PIN_1
#define moveYsub_port GPIOB
#define moveZplus_pin GPIO_PIN_2
#define moveZplus_port GPIOB
#define moveZsub_pin GPIO_PIN_10
#define moveZsub_port GPIOB
// Start - stop - reset
#define start_pin GPIO_PIN_11
#define start_port GPIOB
#define stop_pin GPIO_PIN_12
#define stop_port GPIOB
#define reset_pin GPIO_PIN_13
#define reset_port GPIOB
// Led
#define led_pin GPIO_PIN_14
#define led_port GPIOB
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
