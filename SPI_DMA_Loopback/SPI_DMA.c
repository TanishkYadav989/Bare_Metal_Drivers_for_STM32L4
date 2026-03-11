#include "SPI_DMA.h"
#include "stm32l476xx.h"
void GPIOinit_SDMA(){
    RCC->AHB2ENR|=3UL;
    GPIOA->MODER&= ~((0x3FU)<<10);
    GPIOA->MODER|=(0x2A)<<10;
    GPIOB->MODER&= ~(3UL<<12);
    GPIOB->MODER|=2UL<<12;
    GPIOA->OTYPER&= ~((0x7U)<<5);
    GPIOB->OTYPER&= ~(1UL<<6);
    GPIOA->PUPDR&= ~((0x3FU)<<10);
    GPIOB->PUPDR&= ~(3UL<<12);
    GPIOA->AFR[0]&= ~((0xFFFU)<<20);
    GPIOA->AFR[0]|=(0x555U)<<20;
}
void CS_L(){
    GPIOB->ODR&= ~(1UL<<6);
}
void CS_H(){
    GPIOB->ODR|=1UL<<6;
}
void GPIO_Status(){                            //*This whole function configures the LED Setup for Error Detection the get a visual feedback from them*//
    RCC->AHB2ENR|=1UL<<2;
    GPIOC->MODER&= ~(((0xFFU)<<16)|(3UL<<24));
    GPIOC->MODER|= (((0x55U)<<16)|1UL<<24);
    GPIOC->OTYPER&= ~((15UL<<8)|(1UL<<12));
    GPIOC->PUPDR&= ~(((0xFU)<<20)|(3UL<<24));
    GPIOC->OSPEEDR&= ~(((0xFU)<<20)|(3UL<<24));
}
void SPI_DMA_init(SPI_TypeDef* SPIx){
    RCC->APB2ENR|=1UL<<12;
    RCC->APB2RSTR|=1UL<<12;
    RCC->APB2RSTR&= ~(1UL<<12);

    SPIx->CR1&= ~(1UL<<6);
    SPIx->CR1&= ~((3UL<<14)|(1UL<<13));
    SPIx->CR1&= ~((0xFU)<<7);
	  SPIx->CR1|=3UL<<8;
    SPIx->CR1&= ~((0x7U)<<3);
	  SPIx->CR1|=(0x7U)<<3;
    SPIx->CR1|=((1UL)|(1UL<<2));

    SPIx->CR2|=(0x7U)<<8;
    SPIx->CR2&= ~((3UL<<2)|(1UL<<4));
    SPIx->CR2|=((1UL<<5)|(1UL<<12));
    NVIC->IPR[35]=0x10;
    NVIC->ISER[1]|=1UL<<3;
}
void SDMA_init(){
    RCC->AHB1ENR|=1UL;
    DMA1_Channel2->CCR=0;
    DMA1_Channel3->CCR=0;

    DMA1_Channel2->CCR&= ~1UL;
    DMA1_Channel2->CCR|=((1UL<<1)|(1UL<<3));
    DMA1_Channel2->CCR&= ~((0xFU)<<8);
    DMA1_Channel2->CCR|=3UL<<12;
    DMA1_Channel2->CCR|=1UL<<7;
    DMA1_Channel2->CCR&= ~1UL<<6;
    DMA1_Channel2->CCR&= ~(1UL<<5);
    DMA1_Channel2->CCR&= ~(1UL<<4);
    DMA1_Channel3->CCR&= ~1UL;
    DMA1_Channel3->CCR|=((1UL<<1)|(1UL<<3));
    DMA1_Channel3->CCR&= ~((0xFU)<<8);
    DMA1_Channel3->CCR|=3UL<<12;
    DMA1_Channel3->CCR|=1UL<<7;
    DMA1_Channel3->CCR&= ~1UL<<6;
    DMA1_Channel3->CCR&= ~(1UL<<5);
    DMA1_Channel3->CCR|=1UL<<4;
    DMA1_CSELR->CSELR&= ~((0xFFU)<<4);
    DMA1_CSELR->CSELR|=(0x11U)<<4;
    NVIC->IPR[12]=0x20;
    NVIC->IPR[13]=0X20;
    NVIC->ISER[0]|=1UL<<12;
    NVIC->ISER[0]|=1UL<<13;
}
void SDMA_WR(uint8_t *tbuffer,uint8_t *rbuffer, uint16_t size){
    DMA1->IFCR|=(0xFU)<<4;
    DMA1->IFCR|=(0xFU)<<8;
    CS_L();
    SPI1->CR2|=1UL;
    DMA1_Channel2->CCR&= ~1UL;
    DMA1_Channel2->CPAR=(uint32_t)&(SPI1->DR);
    DMA1_Channel2->CMAR=(uint32_t)rbuffer;
    DMA1_Channel2->CNDTR=size;
    DMA1_Channel2->CCR|=1UL;
    
    DMA1_Channel3->CCR&= ~1UL;
    DMA1_Channel3->CMAR=(uint32_t)tbuffer;
    DMA1_Channel3->CPAR=(uint32_t)&(SPI1->DR);
    DMA1_Channel3->CNDTR=size-1;
    DMA1_Channel3->CCR|=1UL;
    SPI1->CR2|=1UL<<1;
	  *(volatile uint8_t*)&SPI1->DR=tbuffer[0];
	  SPI1->CR1|=1UL<<6;    
}
void SPI1_IRQHandler(){
    if(SPI1->SR & (1UL<<6)){
        
    }  
}
void DMA1_Channel2_IRQHandler(){
   if((DMA1->ISR & (1UL<<5))){
        if(!(SPI1->SR & (1UL<<7))){
					  DMA1_Channel2->CCR&=~1UL;
					  DMA1_Channel3->CCR&=~1UL;
					  SPI1->CR2&= ~1UL;
            SPI1->CR1&= ~(1UL<<6);
            SPI1->CR2&= ~(1UL<<1);
            CS_H();
        }
   }
	 DMA1->IFCR|=(0xFU)<<8;
   if(DMA1->ISR & (1UL<<7)){GPIOC->ODR|=1UL<<10;}
}
void DMA1_Channel3_IRQHandler(){
	 
	 DMA1->IFCR|=(0xFU)<<8;
}    
