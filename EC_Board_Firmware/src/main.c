/**
  ******************************************************************************
  * @file    main.c
  * @author  skk
  * @version V1.0
  * @date    01/03/2020
  * @brief   Default main function.
  ******************************************************************************
*/

#include "stm32f4xx.h"
#include "led.h"
#include "timer.h"
#include "dbus_rc.h"
#include "robot_pose.h"

extern NDJ6_Controller My_Remote;

int main(void) {

    LED_Init();
    RC_Init();
    TIM2_Init(20000, 45);
    TIM4_Init(20000, 45);
    TIM5_Init(20000, 45);
    TIM8_Init(20000, 90);

	for(;;) {
        SKI_ROBOT_STAND();
	}
}


