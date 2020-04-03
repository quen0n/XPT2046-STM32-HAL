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


/* Порты работы тачскрина */
//Порт прерывания должен быть подтянут к питанию!
#define XPT2046_IRQ_Pin       T_IRQ_Pin
#define XPT2046_IRQ_GPIO_Port T_IRQ_GPIO_Port
#define XPT2046_CS_Pin        T_CS_Pin
#define XPT2046_CS_GPIO_Port  T_CS_GPIO_Port

/* Параметры SPI */
//Включить контроль скорости работы SPI
//Если включено, то скорость SPI будет автоматически переключаться на безопасную
#define XPT2046_SPI_PARAM_CONTROL
//Делитель частоты работы SPI
#define XPT2046_SPI_PRESCALER SPI_BAUDRATEPRESCALER_64 

/* Прочие настройки */
//Нормальное количество выборок с тачскрина для усреднения
#define XPT2046_SAMPLES 16
//Минимальное количество выборок с тачскрина для усреднения
//При меньшем значении функция вернёт статус T_noTouch
#define XPT2046_MIN_SAMPLES 8
//Время длинного удерживания в мс
#define XPT2046_LONGPRESS_TIME 2500

/* Ориентации дисплея */
typedef enum {
	XPT2046_LANDSCAPE,
	XPT2046_PORTRAIT,
	XPT2046_LANDSCAPE_180,
	XPT2046_PORTRAIT_180,
} touchOrienation;

/* Объект нажатия на тачскрин */
//Состояния нажатий тачскрина
typedef enum {
	T_noTouch,				//Нет нажатия
	T_pressed,				//Нажат (передний фронт /)
	T_shortHoldDown,	//Короткое удерживание (-)
	T_longHoldDown,		//Длинное удерживание (-)
	T_released, 			//Отпущено (задний фронт \)
} touchStates;
//Объект, содержащий координаты и состояние
typedef struct {
	uint16_t x;
	uint16_t y;
	touchStates state;
} touch_t;

/* Прототипы функций */
//Функция инициализации тачскрина
void XPT2046_init(SPI_HandleTypeDef *spi, touchOrienation orientation, const uint16_t width, const uint16_t height);
//Проверка нажатия на экран. Возвращает состояние нажатия
touchStates XPT2046_getTouchState(void); 
//Получить координаты нажатия
touch_t XPT2046_getTouch(void);

#endif /* XPT2046_H_ */
