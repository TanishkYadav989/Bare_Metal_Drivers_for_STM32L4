#include "UART.h"
#include "gpio.h"
#include "stm32l476xx.h"
#include <stdint.h>
#define n 100
uint8_t usart_buffer[n];
volatile uint32_t R_counter=0, T_counter=0;
void gpioinit_UART(){
    RCC->AHB2ENR|=(0x1U);                            //*Enabling Clock for GPIO Port A*//
    GPIOA->MODER&=~((0xFU)<<4);                        
    GPIOA->MODER|=(0xA)<<4;                          //*Setting PA2 & PA3 as Alternate Fucntion*//
    GPIOA->OSPEEDR&=~((0xFU)<<4);                      
    GPIOA->OSPEEDR|=(0xA)<<4;                        //*Setting PA2 & PA3 for High Output Speed*//
    GPIOA->PUPDR&=~((0xFU)<<4);                      	//*Clearing PA2 & PA3 for no pull up/no pull down*//
	  GPIOA->PUPDR|=5UL<<4; 
    GPIOA->AFR[0]&=~((0xFFU)<<8);                      //*Clearing AFRL for PA2 & PA3*// 
    GPIOA->AFR[0]|=(0x77U)<<8;                       //*Setting AFRL in PA2 for AF7 & PA3 for AF7*//
}
void UART_init(USART_TypeDef* USARTx, uint16_t baud){
    RCC->APB1ENR1|=RCC_APB1ENR1_USART2EN;            //*Enabling Clock for USART2*// 
    USARTx->CR1&=~(1UL);                               //*Disabling USART2 before Setting its function*//
    USARTx->CR1&=~(1UL<<12);                           //*Clearing M0 bit of CR1 for 8 Data Bits*//
    USARTx->CR1&=~(1UL<<28);                         //*Clearing M1 bit of CR1 for 8 Data Bits*//
    USARTx->CR1|=1UL<<15;                            //*Setting OVER8 for Over-Sampling by 8*//
    USARTx->CR2&=~((0x3U)<<12);                      //*Clearing STOP bits for 1 Stop Bit*//
    USARTx->CR1&=~(1UL<<10);                         //*Disabling polarity Control Bit*//
    USARTx->BRR= baud;                               //*Setting Baud Rate Value by Calculating with Processor Clock*// 
    USARTx->CR1|=3UL<<2;                             //*Enabling Tranmitter & Reciever*//
    USARTx->CR1|=1UL<<5;                             //*Enabling Interrupts for Reciever register not empty*//
    USARTx->CR1&=~(1UL<<7);                            //*Disabling Interrupts for Transmit register empty*//
    USARTx->CR1|=1UL;                                //*Enabling USART*//
    NVIC->IPR[9]=0x10;                               //*Setting Interrupt for highest urgency*//
    NVIC->ISER[1]|=1UL<<6;                           //*Enabling Interrupt (no. 38 on the vector table)*//
}
void Recieve(USART_TypeDef* USARTx, uint8_t *buffer, volatile uint32_t *counter){
    if(USARTx->ISR & (1UL<<5)){                      //*Checks the Recieve register not empty flag is set*//
        buffer[*counter]=USARTx->RDR;                //*Reading from RDR clears the RXNE flag & info in buffer array is recieved*//
        (*counter)++;                                //*position in the array increments after every character is recieved*//
        if((*counter)>=n)                            //*Checking for overflow in the array*//
          *counter =0;                               //*Circular Buffer intialised*//
    }
}
void UART_msg(USART_TypeDef* USARTx, uint8_t *buffer){
	  for(int i=0; buffer[i]!='\0' && i<n ; i++){
			usart_buffer[i]=buffer[i];                    //*Transferring all elements to globally defined usart_buffer array*//
		}
	  USARTx->CR1|=1UL<<7;                                //*Setting TXEIE bit from Control Register 1*//
	  T_counter=0;                                        //*Initialising again for safety*//
    USARTx->TDR=buffer[0];       	                      //*TDR is provided with the info*//
}
void Send(USART_TypeDef* USARTx, uint8_t *buffer, volatile uint32_t *counter){
    if(USARTx->ISR & (1UL<<7)){              
			if(buffer[*counter]!='\0'&& (*counter)<n){          //*Checking for space left in the array*//
          USARTx->TDR= buffer[*counter] & 0xFF; 				//*Providing to TDR clears the TXE flag & info is saved for Transmission*//
          (*counter)++;                                         //*Counter increments for changing array position*//
			}  
        else{
          (*counter)=0;                                         //*Managing buffer overflow with Circular Buffer initialisation*//
          USARTx->CR1&=~(1UL<<7);                                 //*Disabling TXEIE from CR1*//
        }  
    }
}
void USART2_IRQHandler(void){
	  if(USART2->ISR & (1UL<<7)){                               //*If Transmission Flag is set UART will send data*//
	    TOGGLE_LED();                                             
        Send(USART2, usart_buffer, &T_counter);}
    if(USART2->ISR & (1UL<<5))                                  //*If Reading Flag is set UART will recieve data*//
      Recieve(USART2, usart_buffer, &R_counter);                     
}