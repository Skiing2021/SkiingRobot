//
// Created by caesa on 1/6/2021.
//
#include "buzzer.h"

void Buzzer_Init(uint16_t period, uint16_t prescaler) {

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM12, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM12, DISABLE);

    {
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_PinAFConfig(GPIOH, GPIO_PinSource6, GPIO_AF_TIM12);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
        GPIO_Init(GPIOH, &GPIO_InitStructure);
    }

    {
        TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
        TIM_TimeBaseInitStructure.TIM_Period = period - 1;
        TIM_TimeBaseInitStructure.TIM_Prescaler = prescaler - 1;
        TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_TimeBaseInit(TIM12, &TIM_TimeBaseInitStructure);
    }

    {
        TIM_OCInitTypeDef TIM_OCInitStructure;
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
        TIM_OC1Init(TIM12, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);
    }

    {
        TIM_ARRPreloadConfig(TIM12, ENABLE);
        TIM_Cmd(TIM12, ENABLE);
    }

    Buzzer_Off();
}

void Buzzer_On(uint16_t prescaler, uint16_t PWM) {
    TIM12->PSC = prescaler;
    TIM_SetCompare1(TIM12, PWM);
}

void Buzzer_Off(void) {
    TIM_SetCompare1(TIM12, 0);
}
