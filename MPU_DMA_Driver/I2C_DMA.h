#ifndef IDMA_H
#define IDMA_H
#include "stm32l476xx.h"
void GPIO_init_IDMA();
void GPIO_Istatus();
void waitline(I2C_TypeDef* I2Cx);
void I2C_DMA_init(I2C_TypeDef* I2Cx, uint8_t ownadd);
void IDMA_init(); 
void I2C_transmit(uint8_t *data, uint16_t size, uint8_t addr);
void I2C_receive(uint8_t add, uint8_t *buffer,uint16_t size);
void I2C1_EV_IRQHandler();
void I2C1_ER_IRQHandler();
void DMA1_Channel6_IRQHandler();
void DMA1_Channel7_IRQHandler();
#endif