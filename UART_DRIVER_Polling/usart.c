#include "usart2.h"
#include "stm32l476xx.h"
#include <stdint.h>
void gpioinit_usart(){
    RCC->AHB2ENR|=(0x1U);                            //*Enabling Clock for GPIO Port A*//
    GPIOA->MODER&=~(0xFU)<<4;                        
    GPIOA->MODER|=(0xA)<<4;                          //*Setting PA2 & PA3 as Alternate Fucntion*//
    GPIOA->OSPEEDR&=~(0xFU)<<4;                      
    GPIOA->OSPEEDR|=(0xA)<<4;                        //*Setting PA2 & PA3 for High Output Speed*//
    GPIOA->PUPDR&=~(0xFU)<<4;                        //*Clearing PA2 & PA3 for no pull up/no pull down*//
    GPIOA->AFR[0]&=~(0xFFU)<<8;                      //*Clearing AFRL for PA2 & PA3*// 
    GPIOA->AFR[0]|=(0x77U)<<8;                       //*Setting AFRL in PA2 for AF7 & PA3 for AF7*//
}
void usartinit(USART_TypeDef* USARTx, uint16_t baud){
    RCC->APB1ENR1|=(0x1)<<17;                        //*Enabling Clock for USART2*// 
    USARTx->CR1&=~1UL;                               //*Disabling USART2 before Setting its function*//
    USARTx->CR1&=~1UL<<12;                           //*Clearing M0 bit of CR1 for 8 Data Bits*//
    USARTx->CR1&=~1UL<<28;                           //*Clearing M1 bit of CR1 for 8 Data Bits*//
    USARTx->CR1|=1UL<<15;                            //*Setting OVER8 for Over-Sampling by 8*//
    USARTx->CR2&=~(0x3U)<<12;                        //*Clearing STOP bits for 1 Stop Bit*//
    USARTx->CR1&=~1UL<<10;                           //*Disabling polarity Control Bit*//
    USARTx->BRR= baud;                               //*Setting Baud Rate Value by Calculating with Processor Clock*// 
    USARTx->CR1|=3UL<<2;                             //*Enabling Tranmitter & Reciever*//
    USARTx->CR1|=1UL;                                //*Enabling USART*//
    while((USART2->ISR & ((0x3)<<21))==0);           //*Waiting for Reciever Enable Ack & Transmitter Enable Ack Flags*//
}
void USART_Write(USART_TypeDef* USARTx, uint8_t *buffer, uint32_t bytes){
    for(unsigned int i=0;i<bytes;i++){
        while(!(USARTx->ISR & (1UL<<7)));            //*Waiting for Transmission Data Register Empty Flag*//
        USARTx->TDR=buffer[i] & 0xFF;                //*Contains Character to be Transmitted With Buffer Array*//
    }
    while(!(USARTx->ISR & (1UL<<6)));                //*Waiting for Transmission Complete Flag*//
    USARTx->ICR|=1UL<<6;                             //*Setting Transfer Complete Clear Flag clears the Flag*//
}
void USART_Read(USART_TypeDef* USARTx, uint8_t *buffer, uint32_t bytes){
    for(unsigned int i=0;i<bytes;i++){
        while(!(USARTx->ISR & (1UL<<5)));            //*Waiting for Read data Register not Empty Flag*//
        buffer[i]=USARTx->RDR;                       //*Reading to RDR sets it, Data recieved is Stored in the buffer Array*//
    }
}