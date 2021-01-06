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
#include "dr16_dbus_rc.h"
#include "buzzer.h"
#include "robot_pose.h"

extern NDJ6_Controller My_Remote;

int main(void) {

    LED_Init();
    Buzzer_Init(30000, 90);
    RC_Init();
    TIM2_Init(20000, 90);
    TIM4_Init(20000, 90);
    TIM5_Init(20000, 90);
    TIM8_Init(20000, 180);

//    Buzzer_On(90, 10000);

	for(;;) {
//	    if (My_Remote.Joystick.Switch_0 = 1);
//        if (My_Remote.Joystick.Switch_0 = 2) {
//            Buzzer_Off();
//        }
//        SKI_ROBOT_CALI_MODE(My_Remote.Joystick.Channel_3);
        SKI_ROBOT_NEUTRAL();
	}
}


