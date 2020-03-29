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

#include <stdio.h>
#include "XPT2046.h"

#define READ_X 0x90
#define READ_Y 0xD0


#ifdef XPT2046_SPI_PARAM_CONTROL
static void _spi_init(SPI_HandleTypeDef *spi) {
  spi->Init.Mode = SPI_MODE_MASTER;
  spi->Init.Direction = SPI_DIRECTION_2LINES;
  spi->Init.DataSize = SPI_DATASIZE_8BIT;
  spi->Init.CLKPolarity = SPI_POLARITY_LOW;
  spi->Init.CLKPhase = SPI_PHASE_1EDGE;
  spi->Init.NSS = SPI_NSS_SOFT;
  spi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  spi->Init.FirstBit = SPI_FIRSTBIT_MSB;
  spi->Init.TIMode = SPI_TIMODE_DISABLE;
  spi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  spi->Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(spi) != HAL_OK)
  {
    Error_Handler();
  }
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

uint8_t XPT2046_TouchPressed(void)
{
    return HAL_GPIO_ReadPin(XPT2046_IRQ_GPIO_Port, XPT2046_IRQ_Pin) == GPIO_PIN_RESET;
}

uint8_t XPT2046_TouchGetCoordinates(uint16_t* x, uint16_t* y)
{
	#ifdef XPT2046_SPI_PARAM_CONTROL
		_spi_init(&XPT2046_SPI_PORT);
	#endif
    static const uint8_t cmd_read_x[] = { READ_X };
    static const uint8_t cmd_read_y[] = { READ_Y };
    static const uint8_t zeroes_tx[] = { 0x00, 0x00 };

    XPT2046_TouchSelect();

    uint32_t avg_x = 0;
    uint32_t avg_y = 0;
    uint8_t nsamples = 0;

    for(uint8_t i = 0; i < 16; i++)
    {
        if(!XPT2046_TouchPressed())
            break;

        nsamples++;

        HAL_SPI_Transmit(&XPT2046_SPI_PORT, (uint8_t*)cmd_read_y, sizeof(cmd_read_y), HAL_MAX_DELAY);
        uint8_t y_raw[2];
        HAL_SPI_TransmitReceive(&XPT2046_SPI_PORT, (uint8_t*)zeroes_tx, y_raw, sizeof(y_raw), HAL_MAX_DELAY);

        HAL_SPI_Transmit(&XPT2046_SPI_PORT, (uint8_t*)cmd_read_x, sizeof(cmd_read_x), HAL_MAX_DELAY);
        uint8_t x_raw[2];
        HAL_SPI_TransmitReceive(&XPT2046_SPI_PORT, (uint8_t*)zeroes_tx, x_raw, sizeof(x_raw), HAL_MAX_DELAY);

        avg_x += (((uint16_t)x_raw[0]) << 8) | ((uint16_t)x_raw[1]);
        avg_y += (((uint16_t)y_raw[0]) << 8) | ((uint16_t)y_raw[1]);
    }

    XPT2046_TouchUnselect();

    if(nsamples < 16)
        return 0;

    uint32_t raw_x = (avg_x / 16);
    uint32_t raw_y = (avg_y / 16);
		
		// Uncomment this line to calibrate touchscreen:
    //printf("raw_x = %d, raw_y = %d\r\n", (int) raw_x, (int) raw_y);
		
		if(raw_x < XPT2046_MIN_RAW_X) raw_x = XPT2046_MIN_RAW_X;
    if(raw_x > XPT2046_MAX_RAW_X) raw_x = XPT2046_MAX_RAW_X;
    if(raw_y < XPT2046_MIN_RAW_Y) raw_y = XPT2046_MIN_RAW_Y;
    if(raw_y > XPT2046_MAX_RAW_Y) raw_y = XPT2046_MAX_RAW_Y;



    *x = (raw_x - XPT2046_MIN_RAW_X) * XPT2046_SCALE_X / (XPT2046_MAX_RAW_X - XPT2046_MIN_RAW_X);
    *y = (raw_y - XPT2046_MIN_RAW_Y) * XPT2046_SCALE_Y / (XPT2046_MAX_RAW_Y - XPT2046_MIN_RAW_Y);

    return 1;
}
