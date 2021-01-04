//
// Created by caesarw on 11/14/20.
//

#ifndef BUTTON_LIGHT_STM32F4VE_DBUS_RC_H
#define BUTTON_LIGHT_STM32F4VE_DBUS_RC_H

#include "stm32f4xx.h"

/* ----------------------- RC Channel Definition---------------------------- */
#define RC_CH_VALUE_MIN              ((uint16_t)364 )
#define RC_CH_VALUE_OFFSET           ((uint16_t)1024)
#define RC_CH_VALUE_MAX              ((uint16_t)1684)
/* ************************************************************************* */


/* ----------------------- RC Switch Definition----------------------------- */
#define RC_SW_UP                     ((uint16_t)1)
#define RC_SW_MID                    ((uint16_t)3)
#define RC_SW_DOWN                   ((uint16_t)2)
/* ************************************************************************* */


/* ----------------------- PC Key Definition-------------------------------- */
#define KEY_PRESSED_OFFSET_W         ((uint16_t)0x01<<0)
#define KEY_PRESSED_OFFSET_S         ((uint16_t)0x01<<1)
#define KEY_PRESSED_OFFSET_A         ((uint16_t)0x01<<2)
#define KEY_PRESSED_OFFSET_D         ((uint16_t)0x01<<3)
#define KEY_PRESSED_OFFSET_Q         ((uint16_t)0x01<<4)
#define KEY_PRESSED_OFFSET_E         ((uint16_t)0x01<<5)
#define KEY_PRESSED_OFFSET_SHIFT     ((uint16_t)0x01<<6)
#define KEY_PRESSED_OFFSET_CTRL      ((uint16_t)0x01<<7)
/* ************************************************************************* */


/* ----------------------- Data Struct ------------------------------------- */
typedef struct {
    struct {
        int16_t Channel_0;
        int16_t Channel_1;
        int16_t Channel_2;
        int16_t Channel_3;
        int16_t Channel_4;
        uint8_t Switch_0;
        uint8_t Switch_1;
    } Joystick;
    struct {
        int16_t x;
        int16_t y;
        int16_t z;
        uint8_t L_Down;
        uint8_t R_Down;
    } Mouse;
    struct {
        uint16_t Key_Value;
    } Keyboard;
} NDJ6_Controller;

/* ************************************************************************* */

void RC_Init(void);

void DMA2_Stream2_IRQHandler(void);

#endif //BUTTON_LIGHT_STM32F4VE_DBUS_RC_H
