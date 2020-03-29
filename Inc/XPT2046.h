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

/* Параметры SPI */
//Включить контроль скорости работы SPI
//Если включено, то скорость SPI будет автоматически переключаться на безопасную
#define XPT2046_SPI_PARAM_CONTROL
//Делитель частоты работы SPI
#define XPT2046_SPI_PRESCALER SPI_BAUDRATEPRESCALER_64 

/* Порты работы тачскрина */
//Порт прерывания должен быть подтянут к питанию!
#define XPT2046_IRQ_Pin       T_IRQ_Pin
#define XPT2046_IRQ_GPIO_Port T_IRQ_GPIO_Port
#define XPT2046_CS_Pin        T_CS_Pin
#define XPT2046_CS_GPIO_Port  T_CS_GPIO_Port

/* Параметры ориентации */
//TODO: Сделать установку параметров при инициализации
#define XPT2046_SCALE_X 320
#define XPT2046_SCALE_Y 240

/* Калибровочные значения дисплея */
#define XPT2046_MIN_RAW_X 1500
#define XPT2046_MAX_RAW_X 30000
#define XPT2046_MIN_RAW_Y 1800
#define XPT2046_MAX_RAW_Y 29000

/* Прототипы функций */
//Функция инициализации тачскрина
//В аргументе указывается интерфейс SPI
void XPT2046_init(SPI_HandleTypeDef *spi);
//Проверка нажатия на экран. Возвращает истину если есть прикосновение
uint8_t XPT2046_TouchPressed(void); 
//Получить координаты нажатия
//TODO: Сделать нормальный обработчик нажатия на экран с прерываниями и без
//TODO: Возвращать объект с координатами
uint8_t XPT2046_TouchGetCoordinates(uint16_t* x, uint16_t* y);

#endif /* XPT2046_H_ */
