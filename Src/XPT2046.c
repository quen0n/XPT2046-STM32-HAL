/*
*========================================================================================================
*
* File                : XPT2046.c
* Hardware Environment: STM32F446
* Build Environment   : Keil uVision 5
* Version             : 1.0
* By                  : Victor Nikitchuk & Andriy Honcharenko
*
*    Основано на примере https://github.com/taburyak/STM32-touchscreen-XPT2046-HAL-SPI
*
*========================================================================================================
*/

#include "XPT2046.h"

#define READ_X 0x90
#define READ_Y 0xD0

/* Параметры ориентации */
//TODO: Сделать установку параметров при инициализации
//TODO: Ориентация дисплея
#define XPT2046_SCALE_X 320
#define XPT2046_SCALE_Y 240

/* Калибровочные значения дисплея */
#define XPT2046_MIN_RAW_X 1500
#define XPT2046_MAX_RAW_X 30000
#define XPT2046_MIN_RAW_Y 1800
#define XPT2046_MAX_RAW_Y 29000

/* Глобальные переменные */
//Указатель на интерфейс SPI
static SPI_HandleTypeDef *_spi;
//Текущее состояние нажатия на тачскрин
static touchStates touchState = T_noTouch;

//Применение безопасных параметров SPI
#ifdef XPT2046_SPI_PARAM_CONTROL
static void _spi_init(void) {
  _spi->Init.Mode = SPI_MODE_MASTER;
  _spi->Init.Direction = SPI_DIRECTION_2LINES;
  _spi->Init.DataSize = SPI_DATASIZE_8BIT;
  _spi->Init.CLKPolarity = SPI_POLARITY_LOW;
  _spi->Init.CLKPhase = SPI_PHASE_1EDGE;
  _spi->Init.NSS = SPI_NSS_SOFT;
	//Делитель частоты SPI. Установите безопасное значение в XPT2046.h
  _spi->Init.BaudRatePrescaler = XPT2046_SPI_PRESCALER;
  _spi->Init.FirstBit = SPI_FIRSTBIT_MSB;
  _spi->Init.TIMode = SPI_TIMODE_DISABLE;
  _spi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  _spi->Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(_spi) != HAL_OK) Error_Handler();
}
#endif

static void XPT2046_TouchSelect(void)
{
    HAL_GPIO_WritePin(XPT2046_CS_GPIO_Port, XPT2046_CS_Pin, GPIO_PIN_RESET);
}

static void XPT2046_TouchUnselect(void)
{
    HAL_GPIO_WritePin(XPT2046_CS_GPIO_Port, XPT2046_CS_Pin, GPIO_PIN_SET);
}

touchStates XPT2046_getTouchState(void) {
	//Если состояние "нажат", то смена состояние на "удержание" и возврат "нажат"
	if(touchState == T_pressed) {
		touchState = T_holdDown;
		return T_pressed;
	}
	//Если состояние "отпущен", то смена состояния на "нет касания" и возврат "отпущен"
	if(touchState == T_released) {
		touchState = T_noTouch;
		return T_released;
	}
	//Иначе просто возврат состояния
	return touchState;
}
//Функция инициализации тачскрина
//В аргументе указывается интерфейс SPI
void XPT2046_init(SPI_HandleTypeDef *spi) {
	_spi = spi;
}

//Функция получения координат нажатия на экран
touch_t XPT2046_getTouch(void) {
	/* Переменные и константы */
	//Массивы для передачи и приёма данных
	static const uint8_t cmd_read_x[] = {READ_X};
	static const uint8_t cmd_read_y[] = {READ_Y};
	static const uint8_t zeroes_tx[] = {0x00, 0x00};
	//Переменная, возвращаемая из функции
	static touch_t touch = {0,0,T_noTouch}; //Переменная сохраняет предыдущие координаты
	
	/* Начало работы по SPI */
	//Если включен контроль скорости SPI, то сохранение параметров и установка правильных значений
	#ifdef XPT2046_SPI_PARAM_CONTROL
	SPI_HandleTypeDef old_spi = *_spi;//Сохранение старых параметров SPI
	_spi_init(); //Инициализация с правильными параметрами
	#endif
	//Активация тачскрина	
	XPT2046_TouchSelect();
	
	/* Выборка и усреднение значений из тачскрина */
	uint32_t avg_x = 0; //Среднее значение X
	uint32_t avg_y = 0;	//Среднее значение Y
	uint8_t nsamples = 0; //Количество сделанных выборок
	
	//Цикл выборки
	for(uint8_t i = 0; i < XPT2046_SAMPLES; i++)	{
		if(HAL_GPIO_ReadPin(XPT2046_IRQ_GPIO_Port, XPT2046_IRQ_Pin) == GPIO_PIN_SET) break; //Если тачскрин был отпущен, то выборка не происходит
		//Получение значений по Y
		HAL_SPI_Transmit(_spi, (uint8_t*)cmd_read_y, sizeof(cmd_read_y), HAL_MAX_DELAY);
		uint8_t y_raw[2];
		HAL_SPI_TransmitReceive(_spi, (uint8_t*)zeroes_tx, y_raw, sizeof(y_raw), HAL_MAX_DELAY);
		//Получение значений по X
		HAL_SPI_Transmit(_spi, (uint8_t*)cmd_read_x, sizeof(cmd_read_x), HAL_MAX_DELAY);
		uint8_t x_raw[2];
		HAL_SPI_TransmitReceive(_spi, (uint8_t*)zeroes_tx, x_raw, sizeof(x_raw), HAL_MAX_DELAY);
		avg_y += (((uint16_t)y_raw[0]) << 8) | ((uint16_t)y_raw[1]);
		avg_x += (((uint16_t)x_raw[0]) << 8) | ((uint16_t)x_raw[1]);
		//Инкримент счётчика выборок
		nsamples++; 
	}
	
	/* Окончание работы по SPI */
	XPT2046_TouchUnselect();
	//Восстановление старых параметров
	#ifdef XPT2046_SPI_PARAM_CONTROL
	*_spi = old_spi; 
	//Инициализация с старыми параметрами
  if (HAL_SPI_Init(_spi) != HAL_OK) Error_Handler();
	#endif
	/* Вычисление значений координат */
	//Если количество реальных выборок меньше установленного значения, то возврат предыдущих координат
	if(nsamples < XPT2046_MIN_SAMPLES) {
		//Если состояние "нет касания", то фильтрация случайного касания
		if (touchState == T_noTouch) {
			touch.state = T_noTouch;
			return touch;
		}
		//Иначе установка состояния "отпущен"
		touchState = T_released;
		touch.state = T_released;
		return touch;
	}
	//Установка состояния "нажат" если было состояние "нет нажатия"
	if(touchState == T_noTouch) touchState = T_pressed;
	//Усреднение значений 
	uint32_t raw_x = (avg_x / nsamples);
	uint32_t raw_y = (avg_y / nsamples);
	//Ограничение значений координат
	//TODO: Устанавливать состояние "отпущен" если значения за возможными пределами
	if(raw_x < XPT2046_MIN_RAW_X) raw_x = XPT2046_MIN_RAW_X;
	if(raw_x > XPT2046_MAX_RAW_X) raw_x = XPT2046_MAX_RAW_X;
	if(raw_y < XPT2046_MIN_RAW_Y) raw_y = XPT2046_MIN_RAW_Y;
	if(raw_y > XPT2046_MAX_RAW_Y) raw_y = XPT2046_MAX_RAW_Y;
	//Вычисление реальных значений X и Y
	touch.x = (raw_x - XPT2046_MIN_RAW_X) * XPT2046_SCALE_X / (XPT2046_MAX_RAW_X - XPT2046_MIN_RAW_X);
	touch.y = (raw_y - XPT2046_MIN_RAW_Y) * XPT2046_SCALE_Y / (XPT2046_MAX_RAW_Y - XPT2046_MIN_RAW_Y);
	touch.state = XPT2046_getTouchState();
	//Возврат значений
	return touch;
}