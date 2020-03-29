/*
*========================================================================================================
*
* File                : XPT2046.h
* Hardware Environment: STM32F446
* Build Environment   : Keil uVision 5
* Version             : 1.0
* By                  : Victor Nikitchuk & Andriy Honcharenko
*
*    Основано на примере https://github.com/taburyak/STM32-touchscreen-XPT2046-HAL-SPI
*
*========================================================================================================
*/

#ifndef XPT2046_H_
#define XPT2046_H_
#include "main.h"

#define XPT2046_SPI_PARAM_CONTROL


// Warning! Use SPI bus with < 2.5 Mbit speed, better ~650 Kbit to be save.
#define XPT2046_SPI_PORT hspi1
extern SPI_HandleTypeDef XPT2046_SPI_PORT;

#define XPT2046_IRQ_Pin       T_IRQ_Pin
#define XPT2046_IRQ_GPIO_Port T_IRQ_GPIO_Port
#define XPT2046_CS_Pin        T_CS_Pin
#define XPT2046_CS_GPIO_Port  T_CS_GPIO_Port

// change depending on screen orientation
#define XPT2046_SCALE_X 320
#define XPT2046_SCALE_Y 240

// to calibrate uncomment UART_Printf line in ili9341_touch.c
#define XPT2046_MIN_RAW_X 1500
#define XPT2046_MAX_RAW_X 30000
#define XPT2046_MIN_RAW_Y 1800
#define XPT2046_MAX_RAW_Y 29000

// call before initializing any SPI devices
uint8_t XPT2046_TouchPressed(void);
uint8_t XPT2046_TouchGetCoordinates(uint16_t* x, uint16_t* y);



#endif /* XPT2046_TOUCH_H_ */
