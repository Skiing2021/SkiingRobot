//
// Created by caesarw on 1/3/21.
//

#ifndef SKI_ROBOT_SPL_TIMER_H
#define SKI_ROBOT_SPL_TIMER_H

#include <stm32f4xx.h>

void TIM2_Init(uint16_t period, uint16_t prescaler);
void TIM4_Init(uint16_t period, uint16_t prescaler);
void TIM5_Init(uint16_t period, uint16_t prescaler);
void TIM8_Init(uint16_t period, uint16_t prescaler);

#endif //SKI_ROBOT_SPL_TIMER_H
