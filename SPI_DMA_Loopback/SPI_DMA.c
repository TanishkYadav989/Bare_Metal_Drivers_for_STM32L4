#include "SPI_DMA.h"
#include "stm32l476xx.h"
void GPIOinit_SDMA(){
    RCC->AHB2ENR|=3UL;                         //*Enabling Clock for GPIO port A & B*//
    GPIOA->MODER&= ~((0x3FU)<<10);
    GPIOA->MODER|=(0x2A)<<10;                  //*Setting PA5 PA6 & PA7 for Alternate Function*//
    GPIOB->MODER&= ~(3UL<<12);
    GPIOB->MODER|=2UL<<12;                     //*Setting PB6 for GP output mode*//
    GPIOA->OTYPER&= ~((0x7U)<<5);              //*Default Push-Pull for PA5, PA6 & PA7*//
    GPIOB->OTYPER&= ~(1UL<<6);                 //*Default Push-Pull for PB6*//
    GPIOA->PUPDR&= ~((0x3FU)<<10);             
    GPIOB->PUPDR&= ~(3UL<<12);
    GPIOA->AFR[0]&= ~((0xFFFU)<<20);           
    GPIOA->AFR[0]|=(0x555U)<<20;               //*Enabling Alternate Function AF5 for PA5, PA6 & PA7*//
}
void CS_L(){
    GPIOB->ODR&= ~(1UL<<6);                    //*Pulling PB6 low*//
}
void CS_H(){
    GPIOB->ODR|=1UL<<6;                        //*PB6 is held high*//
}
void GPIO_Status(){                            //*This whole function configures the LED Setup for Error Detection the get a visual feedback from them*//
    RCC->AHB2ENR|=1UL<<2;
    GPIOC->MODER&= ~(((0xFFU)<<16)|(3UL<<24));
    GPIOC->MODER|= (((0x55U)<<16)|1UL<<24);
}
void SPI_DMA_init(SPI_TypeDef* SPIx){
    RCC->APB2ENR|=1UL<<12;                     //*Enabling Clock for SPI1*// 
    RCC->APB2RSTR|=1UL<<12;                    //*Reseting the SPI1 to discard any Stale bits*//
    RCC->APB2RSTR&= ~(1UL<<12);

    SPIx->CR1&= ~(1UL<<6);                     //*Disabling SPI before configuring*//
    SPIx->CR1&= ~((3UL<<14)|(1UL<<13));        //*Disabling Bi-directional Mode*//
    SPIx->CR1&= ~((0xFU)<<7);                  
	SPIx->CR1|=3UL<<8;                         //*Enabling SSI & SSM*//
    SPIx->CR1&= ~((0x7U)<<3);
	SPIx->CR1|=(0x7U)<<3;                      //*Setting Baud Rate Fpclk/256*//
    SPIx->CR1|=((1UL)|(1UL<<2));               //*SPI Mode 3 with CPOL & CPHA being 1, to capture on the Rising Edge*//

    SPIx->CR2|=(0x7U)<<8;                      //*Setting Data Size as 8-Bit*//
    SPIx->CR2&= ~((3UL<<2)|(1UL<<4));
    SPIx->CR2|=((1UL<<5)|(1UL<<12));           //*Enabling Error interrupt & FRXTH*//
    NVIC->IPR[35]=0x10;
    NVIC->ISER[1]|=1UL<<3;                     //*Enabling SPI interrupt*//
}
void SDMA_init(){
    RCC->AHB1ENR|=1UL;                        //*Enabling Clock for DMA1*//
    DMA1_Channel2->CCR=0;
    DMA1_Channel3->CCR=0;

    DMA1_Channel2->CCR&= ~1UL;                //*Disabling DMA1 Channel 2*//
    DMA1_Channel2->CCR|=((1UL<<1)|(1UL<<3));  //*Enabling TCIE & TEIE*//
    DMA1_Channel2->CCR&= ~((0xFU)<<8);        //*Default PSIZE & MSIZE for 8-Bit*//
    DMA1_Channel2->CCR|=3UL<<12;              //*Setting DMA Channel 2 priority as highest*//
    DMA1_Channel2->CCR|=1UL<<7;               //*Enabling Memory Increment Mode*//
    DMA1_Channel2->CCR&= ~(1UL<<6);           //*Disabling Peripheral Increment Mode*//
    DMA1_Channel2->CCR&= ~(1UL<<5);           //*Disabling Circular Mode*//
    DMA1_Channel2->CCR&= ~(1UL<<4);           //*Transfer Direction from peripheral to memory*//
    DMA1_Channel3->CCR&= ~1UL;                //*Disabling DMA1 Channel 3*//  
    DMA1_Channel3->CCR|=((1UL<<1)|(1UL<<3));  //*Enabling TCIE & TEIE*//
    DMA1_Channel3->CCR&= ~((0xFU)<<8);        //*Default PSIZE & MSIZE for 8-Bit*//
    DMA1_Channel3->CCR|=3UL<<12;              //*Setting DMA Channel 3 priority as highest*//
    DMA1_Channel3->CCR|=1UL<<7;               //*Enabling Memory Increment Mode*//
    DMA1_Channel3->CCR&= ~(1UL<<6);           //*Disabling Peripheral Increment Mode*//
    DMA1_Channel3->CCR&= ~(1UL<<5);           //*Disabling Circular Mode*//
    DMA1_Channel3->CCR|=1UL<<4;               //*Transfer Direction from Memory to Peripheral*//
    DMA1_CSELR->CSELR&= ~((0xFFU)<<4);        
    DMA1_CSELR->CSELR|=(0x11U)<<4;            //*Mapping SPI1's RX & TX to Channel 2 & 3*//
    NVIC->IPR[12]=0x20;                       
    NVIC->IPR[13]=0X20;
    NVIC->ISER[0]|=1UL<<12;                   //*Enabling DMA1 Channel 2's Interrupt*//
    NVIC->ISER[0]|=1UL<<13;                   //*Enabling DMA1 Channel 3's Interrupt*//
}
void SDMA_WR(uint8_t *tbuffer,uint8_t *rbuffer, uint16_t size){
    DMA1->IFCR|=(0xFU)<<4;                    //*Clearing Flags for Both Channels*//
    DMA1->IFCR|=(0xFU)<<8;
    CS_L();                                   //*Pulling CS i.e PB6 line Low for Slave Selection*//
    SPI1->CR2|=1UL;                           //*Setting RXDMAEN for SPI1*//
    DMA1_Channel2->CCR&= ~1UL;
    DMA1_Channel2->CPAR=(uint32_t)&(SPI1->DR); //*Assigning the address of SPI's DR to CPAR*//
    DMA1_Channel2->CMAR=(uint32_t)rbuffer;     //*Assigning the receiver buffer's address to CMAR*//
    DMA1_Channel2->CNDTR=size;                 //*Allocating the size of the buffer to CNDTR*//
    DMA1_Channel2->CCR|=1UL;                   //*Enabling the DMA data Reception stream*//
    
    DMA1_Channel3->CCR&= ~1UL;
    DMA1_Channel3->CMAR=(uint32_t)tbuffer;     //*Assigning the transfer buffer's address to CMAR*//
    DMA1_Channel3->CPAR=(uint32_t)&(SPI1->DR); //*Assigning the address of SPI's DR to CPAR*//
    DMA1_Channel3->CNDTR=size-1;               //*Allocating the size of the buffer to CNDTR*//
    DMA1_Channel3->CCR|=1UL;                   //*Enabling the DMA data Transmission stream*//
    SPI1->CR2|=1UL<<1;                         //*Enabling TXDMAEN for SPI1*//
	*(volatile uint8_t*)&SPI1->DR=tbuffer[0];  //*Manually Transferring the first byte to trigger the transfer*//
	SPI1->CR1|=1UL<<6;                         //*Enabling SPI Peripheral*//
}
void SPI1_IRQHandler(){
    if(SPI1->SR & (1UL<<6)){
       GPIOC->ODR|=1UL<<8;                     //*Visual feedback for the Overrun error*//
    }  
}
void DMA1_Channel2_IRQHandler(){
   if((DMA1->ISR & (1UL<<5))){                 //*Checking for the Transfer Complete Flag*//
        if(!(SPI1->SR & (1UL<<7))){            //*Check if the Busy flag for SPI is not set*//
		   DMA1_Channel2->CCR&=~1UL;           //*Disabling DMA1 Channel 2*//
		   DMA1_Channel3->CCR&=~1UL;           //*Disabling DMA1 Channel 3*//
		   SPI1->CR2&= ~1UL;                   //*Disabling RXDMAEN for SPI1*//
           SPI1->CR1&= ~(1UL<<6);              //*Disabling SPI1 peripheral*//
           SPI1->CR2&= ~(1UL<<1);              //*Disabling TXDMAEN for SPI1*//
           CS_H();                             //*Pulling CS pin high for Slave De-selecting*//
        }
   }
	DMA1->IFCR|=(0xFU)<<8;                     //*Clearing flags for Channel 2 & 3*//
   if(DMA1->ISR & (1UL<<7)){GPIOC->ODR|=1UL<<10;}   //*Visual Feedback for the Transfer Error Flag*//
}
void DMA1_Channel3_IRQHandler(){
	DMA1->IFCR|=(0xFU)<<8;                     //*Clearing flags for Channel 2 & 3*//
}      
