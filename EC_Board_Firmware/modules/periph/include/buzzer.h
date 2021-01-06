//
// Created by caesa on 1/6/2021.
//

#ifndef SKI_ROBOT_SPL_BUZZER_H
#define SKI_ROBOT_SPL_BUZZER_H

#include "stm32f4xx.h"

extern void Buzzer_Init(uint16_t period, uint16_t prescaler);

extern void Buzzer_On(uint16_t prescaler, uint16_t PWM);

extern void Buzzer_Off(void);

#endif //SKI_ROBOT_SPL_BUZZER_H
