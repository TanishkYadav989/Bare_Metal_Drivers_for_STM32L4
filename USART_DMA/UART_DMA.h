#ifndef UDMA_H
#define UDMA_H
#include "stm32l476xx.h"
#include "stdint.h"
void GPIO_init_UDMA();
void GPIO_status();
void UART_DMA_init(USART_TypeDef* USARTx, uint16_t baud);
void UDMA_init();
void UDMA_send(uint8_t *buffer, uint16_t size);
void DMA1_Channel7_IRQHandler();
#endif