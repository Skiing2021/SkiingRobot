//
// Created by caesarw on 1/4/21.
//

#ifndef SKI_ROBOT_SPL_ROBOT_POSE_H
#define SKI_ROBOT_SPL_ROBOT_POSE_H

#include "stm32f4xx.h"

void SKI_ROBOT_NEUTRAL(void);
void SKI_ROBOT_STAND(void);
void SKI_ROBOT_CALI_MODE(int16_t PWM);
uint32_t MOTOR_RANGE_MAPPING(int16_t src);

#endif //SKI_ROBOT_SPL_ROBOT_POSE_H
