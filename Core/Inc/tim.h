/*
 * tim.h
 *
 *  Created on: Sep 7, 2024
 *      Author: apple
 */

#ifndef INC_TIM_H_
#define INC_TIM_H_

#include "main.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void MX_TIM4_Init(void);

#endif /* INC_TIM_H_ */
