#include "SPIx.h"
#include "stm32l476xx.h"
#include <stdint.h>
volatile uint8_t *data_t=0;                       //*Initialising data buffer for the Transmission*//
volatile uint8_t *data_r=0;                       //*Initialising data buffer for receiving data*//
volatile uint8_t ind_t=0;
volatile uint8_t ind_r=0;
volatile uint8_t sizex=0;
volatile uint8_t ready;
void GPIO_config(){
    RCC->AHB2ENR|=3UL;                           //*Enabling Clock for GPIO Port A & B*//
    GPIOB->MODER&= ~(3UL<<12);                 
    GPIOB->MODER|=1UL<<12;                       //*Setting PB6 for General Purpose Output Mode (01)*//
    GPIOA->MODER&= ~((0x3FU)<<10);
    GPIOA->MODER|=(0x2AU)<<10;                   //*Setting PA5, PA6 & PA7 for Alternate Functions (10) as they are mapped to SCLK, MOSI & MISO*//
    GPIOB->OTYPER&= ~(1UL<<6);                   //*Clearing bit of PB6 for Push-Pull*//
    GPIOA->OTYPER&= ~((0x5U)<<5);                //*Clearing bits of PA5, PA6 & PA7 for Push-Pull*//
    GPIOA->AFR[0]&= ~((0xFFFU)<<20);               
    GPIOA->AFR[0]|=(0x555U)<<20;                 //*Enabling Alternate Functions of the SPI pins in the AFRL*//
}
void CS_L(){
    GPIOB->ODR&= ~(1UL<<6);                      //*Clearing ODR to pull PB6 down to select device for SPI transmission/reception*//
}
void CS_H(){
    GPIOB->ODR|=1UL<<6;                          //*Setting ODR to pull PB6 high to deselct the device*// 
}
void SPI_init(SPI_TypeDef* SPIx){
    RCC->APB2ENR|=1UL<<12;                       //*Enablinig Peripheral Clock for SPI1*//
    RCC->APB2RSTR|=1UL<<12;                       
    RCC->APB2RSTR&= ~(1UL<<12);                  //*Completing reset of the Peripheral before configuring it*//

    SPIx->CR1&= ~(1UL<<6);                       //*Disabling SPI*//
    SPIx->CR1&= ~(1UL<<15);                      //*Disabling Bi-directional mode*//
    SPIx->CR1&= ~(1UL<<7);                       //*Disabling to Transmit Most Significant Bit first*//
    SPIx->CR1&= ~(3UL<<8);                     
    SPIx->CR1&= ~((3UL<<3)|(1UL<<5));            
	SPIx->CR1|=((1UL<<3)|(1UL<<5));              //*Setting Baud rate Fpclk/64 for slow speed*//
    SPIx->CRCPR= 7;                              //*Setting CRC polynomial even though CRC is disabled, used just for safety*//
    SPIx->CR1&= ~(1UL<<13);                      //*Disabling CRC*//
    SPIx->CR1|=3UL<<8;                           //*Enabling SSI & SSM for Slave selection*//  

    SPIx->CR2&= ~((0xFU)<<8);                      
    SPIx->CR2|=7UL<<8;                           //*Setting data size as 8-bit*//
    SPIx->CR2|=1UL<<12;                          //*Setting FRXTH as the data received is 8-bit*//
    SPIx->CR2|= (1UL<<5);                        //*Error interrupt bit is Enabled*//
    SPIx->CR2&= ~(3UL<<3);                       //*Disabling NSSP & SSOE*//
	
    SPIx->CR1|=3UL;	                             //*Setting Clock Polarity & Clock Phase as to capture on rising edge*//
    SPIx->CR1|=1UL<<2;                           //*Setting the Controller as Master Configuring*//
    SPIx->CR1|=1UL<<6;                           //*Enabling SPI*//
    NVIC->IPR[8]=0x10;                           //*Setting SPI Priority as high*//
    NVIC->ISER[1]|=1UL<<3;                       //*Enabling SPI interrupt*//
}
void SPI_WR(SPI_TypeDef* SPIx, volatile uint8_t *txdata, volatile uint8_t *rxdata, uint8_t size){
    if(SPIx->SR & (1UL<<7))                      //*Check is the line is Idle for Transmission & Reception*//
    return; 
    data_t=txdata;                               //*Mapping the Arguement Transmit Buffer to the Global Transmission buffer*//
	data_r=rxdata;                               //*Mapping the Arguement Receive Buffer to the Global Reception Buffer buffer*//
    ind_t=0;                                     //*Setting buffer index to 0*//
    ind_r=0;
    sizex=size;                                  //*Mapping buffer size to the Globally initialised Size variable*//
	SPIx->CR2|=1UL<<6;                           //*Enabling RXNEIE bit*//
    SPIx->CR2|=1UL<<7;                           //*Enabling TXEIE bit*//
}
void SPI1_IRQHandler(){
    if(SPI1->SR & (1UL)){                        //*Checking Receiver not empty flag in the Status Register*//
		  if(ind_r<sizex){                       //*Checking if index is less than the size*//
				data_r[ind_r++]=(*(volatile uint8_t *)&SPI1->DR) & 0xFF;   //*Reading data from the Data register and also casting it to prevent garbage data as DR is 16 bit register*//
				if(ind_r>=sizex){
					CS_H();                       //*Pulling PB6 high to deselect the pin*//
					SPI1->CR2&= ~(3UL<<6);        //*Disabling TXEIE & RXNEIE bits*//
					ready=1;                      //*Setting Data ready Flag*//
				}
		    }	
	    } 
		if(SPI1->SR & (1UL<<1)){                  //*Checking for the Transmit Empty Flag in SR*//
			if(ind_t<sizex){                      //*checking if Transmission index is less than size*//
				(*(volatile uint8_t *)&SPI1->DR)=data_t[ind_t++] & 0xFF;   //*Transferring data to the Data register and also casting it to prevent garbage data as DR is 16 bit register*//  
			}
			else{
				SPI1->CR2&= ~(1UL<<7);            //*If buffer is full or overflow occured, disabled TXEIE bit*//
			}
		}      		
}