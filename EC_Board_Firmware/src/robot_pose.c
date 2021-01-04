//
// Created by caesarw on 1/4/21.
//

#include "robot_pose.h"

void SKI_ROBOT_STAND(void) {
    TIM_SetCompare1(TIM2, 1500);    // 12
    TIM_SetCompare2(TIM2, 1470);    // 19
    TIM_SetCompare3(TIM2, 1530);    // 16
    TIM_SetCompare4(TIM2, 1100);    // 7

    TIM_SetCompare1(TIM4, 1900);    // 4
    TIM_SetCompare2(TIM4, 500);     // 8
    TIM_SetCompare3(TIM4, 2500);    // 3
    TIM_SetCompare4(TIM4, 1500);    // 9

    TIM_SetCompare1(TIM5, 1500);    // 2
    TIM_SetCompare2(TIM5, 1300);    // 20
    TIM_SetCompare3(TIM5, 1700);    // 15
    TIM_SetCompare4(TIM5, 1950);    // 21

    TIM_SetCompare1(TIM8, 1050);    // 14
    TIM_SetCompare2(TIM8, 1500);    // 22
    TIM_SetCompare3(TIM8, 1500);    // 13
    TIM_SetCompare4(TIM8, 1470);    // 23
}
