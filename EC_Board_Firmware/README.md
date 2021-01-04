# EC Board Firmware For AI Skiing Robot
## Specifications
* This firmware is designed for RoboMaster Development Board Type A __(MCU: STM32F427IIHx)__.  
  Technically, it's possible to use it on any generic STM32F427IIHx platform.
* Implemented Features:  
  * 8-way PWM output via `TIM2`, `TIM4`, `TIM5` and `TIM8`
  * Basic poses (standing)
  * DJI remote control via IRQ and DMA
* TODO:
  * More built-in poses (left, right, forward, backward, etc.)
  * SD card driver
  * IMU and real-time robot pose solving
  
## Hands-on
* Recommended Setup:
  * OS: Linux
  * Compiler / Toolchain: ARM-GCC
  * IDE: Jetbrains CLion
  * Hardware Debugger: CMSIS-DAP via OpenOCD (ST-Link is acceptable)
