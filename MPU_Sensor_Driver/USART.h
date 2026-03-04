#ifndef UART_INT_H
#define UART_INT_H
#include "stm32l476xx.h"
#include <stdio.h>
extern volatile uint32_t R_counter, T_counter;
void gpioinit_UART();
void UART_init(USART_TypeDef* USARTx, uint16_t baud);
void UART_msg(USART_TypeDef* USARTx, uint8_t *buffer);
void send(USART_TypeDef* USARTx, uint16_t *buffer, uint16_t *counter);
void recieve(USART_TypeDef* USARTx, uint16_t *buffer, uint16_t *counter);
void USART2_IRQHandler();
int fputc(int ch, FILE *f);
#endif