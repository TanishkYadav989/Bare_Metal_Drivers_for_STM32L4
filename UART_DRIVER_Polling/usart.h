#ifndef USART2_H
#define USART2_H
#include "stm32l476xx.h"
#include <stdint.h>
void gpioinit_usart();
void usartinit(USART_TypeDef* USARTx, uint16_t baud);
void USART_Write(USART_TypeDef* USARTx, uint8_t *buffer, uint32_t bytes);
void USART_Read(USART_TypeDef* USARTx, uint8_t *buffer, uint32_t bytes);
#endif