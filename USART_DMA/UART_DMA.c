#include "UART_DMA.h"
#include "stm32l476xx.h"
#include <stdint.h>
void GPIO_init_UDMA(){
    RCC->AHB2ENR|=1UL; //*Enabling Clock for GPIO Port A*//
    GPIOA->MODER&= ~((0xFU)<<4);            
    GPIOA->MODER|=(0xAU)<<4; //*Setting Pin A2 & A3 for Alternate Function as they are mapped to USART_TX & USART_RX*//
    GPIOA->OTYPER&= ~(3UL<<2); //*Setting both pins as Push-Pull*//
    GPIOA->PUPDR&= ~((0xFU)<<4); //*No pull up/pull down for PA2 & PA3*//
    GPIOA->AFR[0]&= ~((0xFFU)<<8);
    GPIOA->AFR[0]|=(0x77U)<<8; //*Enabling Alternate function AF7 of PA2 & PA3 in the AFRL*//
}
void GPIO_status(){
    RCC->AHB2ENR|=1UL<<2; //*Enabling Clock for GPIO port C*//
    GPIOC->MODER&= ~(((0xFU)<<20)|(3UL<<24));
    GPIOC->MODER|= ((5UL<<20)|1UL<<24); //*Setting PC10, PC11 & PC12 for GP output mode*//
    GPIOC->OTYPER&= ~(7UL<<10); //*Setting them as push-pull*//
    GPIOC->PUPDR&= ~(((0xFU)<<20)|(3UL<<24)); //*Disabling Pull up/Pull down*//
    GPIOC->OSPEEDR&= ~(((0xFU)<<20)|(3UL<<24)); //*Output Speed is Low*//
}
void UART_DMA_init(USART_TypeDef* USARTx, uint16_t baud){
    RCC->APB1ENR1|=1UL<<17; //*Enabling Peripheral clock for USART2*//
    RCC->APB1RSTR1|=1UL<<17;
    RCC->APB1RSTR1&= ~(1UL<<17); //*Completing Reset of the Peripheral*//

    USARTx->CR1&= ~1UL; //*Disabling UART before configuring*// 
    USARTx->CR1&= ~((1UL<<12)|(1UL<<28)); //*Clearing M0 & M1 for 1 Start bit, 8 data bits & n Stop bit*//
    USARTx->CR1|=1UL<<15; //*Setting Oversampling by 8*//
    USARTx->CR1&= ~(1UL<<10); //*Disabling Parity Control*//
    USARTx->CR1|=(1UL<<6); //*Enabling Transfer Complete Interrupt Flag*//
    USARTx->BRR= baud; //*Providing Baud Rate to BRR via Arguement*//

    USARTx->CR2&= ~(3UL<<12); //*Setting Stop Bit = 1*//
    USARTx->CR3|=3UL<<6; //*Enabling TXDMAEN & RXDMAEN bits to get access to the DMA engine*//
    
    USARTx->CR1|=3UL<<2; //*Enabling Transmitter & Receiver*//
    USARTx->CR1|=1UL;
}
void UDMA_init(){
	RCC->AHB1ENR|=1UL; //*Enabling Clock of DMA1*//

    DMA1_Channel7->CCR&= ~(1UL); //*Disabling DMA1 before configuring it*//
    DMA1_Channel7->CCR&= ~((0xFU)<<8); //*Setting Peripheral size & Memory size for 8 bits*//
    DMA1_Channel7->CCR|=(0xFU)<<1; //*Enabling Transfer Complete, Half Transfer, Transfer Error Interrupt bits & also setting the Direction of transfer i.e Memory to Peripheral*//
    DMA1_Channel7->CCR|=3UL<<12; //*Setting priority level as very high*//
    DMA1_Channel7->CCR&= ~(3UL<<5); //*Disabling Circular mode as Transmitting only once & peripheral increment mode*//
    DMA1_Channel7->CCR|=1UL<<7; //*Enabling memory increment mode to access other values/data in the buffer*//
	DMA1_CSELR->CSELR&= ~((0xFU)<<24);                      
    DMA1_CSELR->CSELR|=(0X2U)<<24; //*Mapping DMA1 ch7 for USART_TX*//
	NVIC->IPR[4]=0x20; //*Setting Interrupt Priority*//
    NVIC->ISER[0]|=1UL<<17; //*Enabling DMA1_Channel7 Interrupt*//
}
void UDMA_send(uint8_t *buffer, uint16_t size){
	DMA1_Channel7->CCR&= ~(1UL); //*Disabling DMA1 again for safety as not fully Configured yet*//   
    DMA1_Channel7->CPAR= (uint32_t) &USART2->TDR; //*Providing the Peripheral Data register Address for transfer destination & casting it*//
    DMA1_Channel7->CMAR= (uint32_t) buffer; //*Providing the Memory Address of the buffer arguement for Transfer origin*//
    DMA1_Channel7->CNDTR=size; //*Alotting the size of the Data to be transmitted*//
    DMA1_Channel7->CCR|=1UL; //*Enabling DMA1*//
}
void DMA1_Channel7_IRQHandler(){
    if(DMA1->ISR & (1UL<<25)){ //*Checking for the Transfer Complete Flag*// 
        GPIOC->ODR|=1UL<<10; //*Lighting up an LED connected to PC10 to get a visual feedback*//
        DMA1->IFCR|=1UL<<25; //*Clearing TCI7 flag*//
    }
    if(DMA1->ISR & (1UL<<26)){ /*Checking for the Half-Transfer Complete Flag*//
        GPIOC->ODR|=1UL<<11; //*Lighting up an LED connected to PC11 to get a visual feedback*//
        DMA1->IFCR|=1UL<<26; //*Clearing HTCI7 flag*//
    }
    if(DMA1->ISR & (1UL<<27)){ //*Checking for the Transfer Error Flag*//
        GPIOC->ODR|=1UL<<12; //*Lighting up an LED connected to PC12 to get a visual feedback*//
	    DMA1->IFCR|=1UL<<27; //*Clearing TEI7 flag*//
    }
}  
