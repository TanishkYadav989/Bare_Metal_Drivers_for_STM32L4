#ifndef UART_INT_H
#define UART_INT_H
#include "stm32l476xx.h"
extern volatile uint32_t R_counter, T_counter;
void gpioinit_usart();
void usartinit(USART_TypeDef* USARTx, uint8_t baud);
void UART_msg(USART_TypeDef* USARTx, uint8_t *buffer);
void send(USART_TypeDef* USARTx, uint8_t *buffer, uint16_t *counter);
void recieve(USART_TypeDef* USARTx, uint8_t *buffer, uint16_t *counter);
void USART2_IRQHandler();
#endif