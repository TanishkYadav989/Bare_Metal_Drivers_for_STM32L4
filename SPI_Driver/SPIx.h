#ifndef SPI_H
#define SPI_H
#include "stm32l476xx.h"
#include <stdint.h>
extern volatile uint8_t ready;
void GPIO_config();
void CS_L();
void CS_H();
void SPI_init(SPI_TypeDef* SPIx);
void SPI_WR(SPI_TypeDef* SPIx, volatile uint8_t *data_t, volatile uint8_t *data_r, uint8_t size);
void SPI_IRQHandler();
#endif