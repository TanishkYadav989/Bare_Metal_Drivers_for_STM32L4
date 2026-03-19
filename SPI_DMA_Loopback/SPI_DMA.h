#ifndef SPI_D
#define SPI_D
#include "stm32l476xx.h"
void GPIOinit_SDMA();
void CS_L();
void CS_H();
void GPIO_Status(); 
void SPI_DMA_init(SPI_TypeDef* SPIx);
void SDMA_init();
void SDMA_WR(uint8_t *tbuffer,uint8_t *rbuffer, uint16_t size);
void SPI1_IRQHandler();
void DMA1_Channel2_IRQHandler();
void DMA1_Channel3_IRQHandler();
#endif                           