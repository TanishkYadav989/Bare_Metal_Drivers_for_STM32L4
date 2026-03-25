#include "I2C_DMA.h"
#include "stm32l476xx.h"
volatile int dready=1; //*Using as flag for detecting Transfer Completion & idle line*//
static uint8_t buff1[2]={0x1C,0x08}; //*This is the Register Address & its Config. of the Accelerometer*//
static uint8_t buff2[1]={0x3B}; //*This is the Register address of the fisrt Read Register*//
uint8_t r[14]; //*Intialising a Receiver buffer to store the MPU Data*//
typedef enum{ //*Initialising an Enum to structure the State Machine Approach in the I2C EV handler*//
	STATE_PWR,
	STATE_ACC,
	STATE_REG,
	STATE_READ,
}State_t;
static State_t state=STATE_PWR;	                 
void GPIO_init_IDMA(){
    RCC->AHB2ENR|=1UL<<1; //*Enabling Clock of GPIO Port B*//
    GPIOB->MODER&= ~((0xFU)<<16);                
    GPIOB->MODER|=(0xAU)<<16; //*Setting PB8 & PB9 for Alternate Functions*//
    GPIOB->OTYPER&= ~(3UL<<8);
	GPIOB->OTYPER|=3UL<<8; //*Setting PB8 & PB9 as Open-Drain Output required for I2C Protocol*//
    GPIOB->PUPDR&= ~((0xFU)<<16); 
    GPIOB->PUPDR|=(0x5UL)<<16; //*Internal Pull up for PB8 & PB9 as open drain required pull up resistors*//
    GPIOB->AFR[1]&= ~(0xFFU);
    GPIOB->AFR[1]|=(0x44U); //*Setting PB8 & PB9 for AF4 in the AFLH Register*//
}
void GPIO_Istatus(){ //*This whole function configures the LED Setup for Error Detection the get a visual feedback from them*//
    RCC->AHB2ENR|=1UL<<2;
    GPIOC->MODER&= ~(((0xFFU)<<16)|(3UL<<24));
    GPIOC->MODER|= (((0x55U)<<16)|1UL<<24);
    GPIOC->OTYPER&= ~((15UL<<8)|(1UL<<12));
    GPIOC->PUPDR&= ~(((0xFU)<<20)|(3UL<<24));
    GPIOC->OSPEEDR&= ~(((0xFU)<<20)|(3UL<<24));
}	
void I2C_DMA_init(I2C_TypeDef* I2Cx, uint8_t ownadd){
    RCC->APB1ENR1|=1UL<<21; //*Enabling Clock of I2C1*//

    I2Cx->CR1&= ~(1UL); //*Disabling the Peripheral before Configuration*//
    I2Cx->CR1&= ~(3UL<<20); //*Disabling Slave Management Bus*//
    I2Cx->CR1|=1UL<<17; //*Disabling Clock Stretching as STM is in Master Configuration*//
	I2Cx->CR1|=1UL<<5; //*Enabling Stop detection interrupt*//
    I2Cx->CR1&= ~((0xFU)<<8); //*Digital Noise Filter Disabled*// 
    I2Cx->CR1&= ~(1UL<<12); //*Analog Noise Filter Enabled*//

    I2Cx->TIMINGR=0; //*Clearing out Timing register*//
    I2Cx->TIMINGR&= ~((0xFU)<<28); //*Setting Prescaler to 0 as the Clock operates on 4MHz*//
    I2Cx->TIMINGR|= ((19UL) | (15UL)<<8 | (0x4U)<<16 | (0x2U)<<20);  //* SCLL= 19, SCLH= 15,SDADEL= 4, SCLDEL= 2 according to the minimum requirements for the sensor*//

    I2Cx->OAR1&=~(1UL<<10); //*Disabling Own Address*//
    I2Cx->OAR1= ownadd; //*Allotting own address via Function Arguement*//
    I2Cx->OAR1|=1UL<<15; //*Setting OA as 7-Bit Address*//
   
	I2Cx->CR1|=1UL<<7; //*Enabling Error Detection Interrupt*//
    I2Cx->CR1|=1UL<<1; //*Enabling TXIE*//
    I2Cx->CR1|=1UL;  //*Enabling I2C1*//
    NVIC->IPR[31]=0x10; //*Setting I2C1 Event Interrupt's priority*//
    NVIC->IPR[32]=0x10; //*Setting I2C1 Error Interrupt's priority*//
    NVIC->ISER[0]|=1UL<<31; //*Enabling I2C1-EV using ISER 0*//
	NVIC->ISER[1]|=1UL; //*Enabling I2C1-ER using ISER 1*//
}
void IDMA_init(){
    RCC->AHB1ENR|=1UL; //*Enabling Clock of DMA1*//
	DMA1_Channel6->CCR=0; //*Resetting DMA1 Channel 6's Control Register*//
	DMA1_Channel7->CCR=0; //*Resetting DMA1 Channel 7's Control Register*//
    
    DMA1_Channel6->CCR&= ~(1UL); //*Disabling DMA1 Channel 6 before configuring it*//
    DMA1_Channel6->CCR&= ~((0xFU)<<8); //*Setting Peripheral size & Memory size to 8-bits*//
    DMA1_Channel6->CCR|=3UL<<12; //*Setting Channel Priority as very high*//
	DMA1_Channel6->CCR&=~(1UL<<6); //*Disabling Peripheral Increment Mode*//
    DMA1_Channel6->CCR|=1UL<<7; //*Enabling Memory Increment Mode*//
    DMA1_Channel6->CCR&=~ (1UL<<5); //*Circular Mode being Disabled*//
    DMA1_Channel6->CCR|=1UL<<4; //*Setting data transfer direction from Memory to Peripheral*//
    DMA1_Channel6->CCR|=((1UL<<1)|(3UL<<2)); //*Enabling Transfer Complete, Half Transfer & Transfer Error Interrupts*//
	DMA1_Channel7->CCR&= ~1UL; //*Disabling DMA1 Channel 7 before configuring it*//
	DMA1_Channel7->CCR&=~((0xFU)<<8); //*Setting Peripheral size & Memory size to 8-bits*//
	DMA1_Channel7->CCR|=3UL<<12; //*Setting Channel Priority as very high*//
	DMA1_Channel7->CCR&=~(1UL<<6); //*Disabling Peripheral Increment Mode*//
    DMA1_Channel7->CCR|=1UL<<7; //*Enabling Memory Increment Mode*//
    DMA1_Channel7->CCR&=~ (1UL<<5); //*Circular Mode being Disabled*//
	DMA1_Channel7->CCR&= ~(1UL<<4); //*Setting data transfer from Peripheral to Memory*// 
	DMA1_Channel7->CCR|=((1UL<<1)|(3UL<<2)); //*Enabling Transfer Complete, Half Transfer & Transfer Error Interrupts*//
    DMA1_CSELR->CSELR&= ~((0xFFU)<<20);        
	DMA1_CSELR->CSELR|=(0x33U)<<20; //*Enabling DMA1 Channel 6 & 7 according to the DMA1 mapping Table*//
    NVIC->IPR[16]=0x20; //*Allotting priority of Channel 6*//
	NVIC->IPR[17]=0x20; //*Allotting priority of Channel 7*//
    NVIC->ISER[0]|=1UL<<16; //*Enabling Channel 6 Interrupt*//
	NVIC->ISER[0]|=1UL<<17; //*Enabling Channel 7 Interrupt*//
}
static void I2C_Start(uint8_t *data, uint16_t size, uint8_t addr){
	uint32_t REG = (I2C1->CR2); //*Allotting Control Register 2 to temporary Register REG for Atomic Setting of CR2*//
	DMA1->IFCR|=(0xFU)<<20; //*Clearing Status Flags of DMA1 Channel 6*//
	DMA1_Channel6->CCR&= ~1UL; //*Disabling Channel 6 again for safety*//
	DMA1_Channel6->CMAR=(uint32_t)data; //*Allotting Memory Address to  CMAR as the source of transfer*//
	DMA1_Channel6->CPAR=(uint32_t)&(I2C1->TXDR); //*Allotting Peripheral's Transmit Data Register Address to CPAR as the destination of transfer*// 
	DMA1_Channel6->CNDTR=size; //*Allotting the size of the Buffer to CNDTR as a counter value*//
	DMA1_Channel6->CCR|=1UL; //*Enabling DMA1 Channel 6*//
	I2C1->CR1|=1UL<<14; //*Enable TX DMA transfer bit to set the TXIS bit for transfer*//
	 
	REG&= ~(0x03FF03FF); //*Clearing CR2 at once*//
    REG&= ~(1UL<<10); //*Setting Write mode for I2C1*// 
	REG|=(((uint32_t)addr<<1)|(uint32_t)(size<<16)); //*Allotting the Slave Address and the bytes for Transfer*//
	REG|=1UL<<25; //*Enabling Autoend to send a STOP bit automatically when transfer is finished*//
	REG|=1UL<<13; //*Setting Start bit to send a START bit to initiate transfer*//
	I2C1->CR2=REG; //*Allotting back to CR2 for Atomic Setting of it*//
}
void I2C_transmit(uint8_t *data, uint16_t size, uint8_t addr){
	if(dready==0){return;}
	dready=0;
	I2C_Start(data,size,addr); //*Initiating transfer from the I2C*//	   
}
static void I2C_read(uint8_t add, uint8_t *buffer,uint8_t size){
	uint32_t REG = (I2C1->CR2); //*Allotting Control Register 2 to temporary Register REG for Atomic Setting of CR2*//
	DMA1->IFCR|=(0xFU)<<24; //*Clearing Status Flags of DMA1 Channel 7*//
	DMA1_Channel7->CCR&= ~1UL; //*Disabling Channel 7 again for safety*//
	DMA1_Channel7->CPAR=(uint32_t)&(I2C1->RXDR); //*Allotting Peripheral's Receive Data Register Address to CPAR as the source of transfer*// 
	DMA1_Channel7->CMAR=(uint32_t)buffer; //*Allotting Memory Address to  CMAR as the destination of transfer*//
	DMA1_Channel7->CNDTR=size; //*Allotting the size of the Buffer to CNDTR as a counter value*//
	DMA1_Channel7->CCR|=1UL; //*Enabling DMA1 Channel 7*//
	I2C1->CR1|=1UL<<15; //*Enable RX DMA transfer bit to set the RXNE flag for transfer*//
	
	REG&= ~(0x03FF03FF); //*Clearing CR2 at once*//
	REG&= ~(1UL<<10);                       
	REG|=1UL<<10; //*Setting Read mode for I2C1*// 
	REG|=(((uint32_t)add<<1)|(uint32_t)(size<<16)); //*Allotting the Slave Address and the bytes for Transfer*//
	REG|=1UL<<25; //*Enabling Autoend to send a STOP bit automatically when transfer is finished*//
	REG|=1UL<<13; //*Setting Start bit to send a START bit to initiate transfer*//
	I2C1->CR2=REG; //*Allotting back to CR2 for Atomic Setting of it*//
}
void I2C_receive(uint8_t add, uint8_t *buffer, uint16_t size){
    if(dready==0){return;}
	dready=0;	
	I2C_read(add,buffer,size); //*Initiating receive sequence*//
}
void I2C1_EV_IRQHandler(){
    if(I2C1->ISR & (1UL<<5)){ //*Checking for STOPF flag in the ISR*//
      I2C1->ICR|=1UL<<5; //*Clearing STOPF flag via ICR*//
        switch(state){ //*Initialising the State Machine's Structural Flow*//
			case STATE_PWR:
				state++; //*When first transfer is completed, machine increments and initiates second transfer*//
				I2C_Start(buff1,2,0x68);
				break;
			case STATE_ACC:
			    state++; //*After Second Transfer Machine increments & initiates third tranfer to sent the Register Address of read registers of the MPU*//
				I2C_Start(buff2,1,0x68);
				break;
			case STATE_REG:
				state++; //*After Third Transfer Machine increments & a read sequence is initiated*//
			    I2C_read(0x68,r,14);
			    break;
			case STATE_READ:                  
				I2C_Start(buff2,1,0x68); //*When the read sequence ends, the Third Transfer for the read register is initiated again and the machine loops back to the read sequence i.e the previous state for continuous reading*//
			    state=STATE_REG;
	     		dready=1;
				break;
		}					
    }
}
void I2C1_ER_IRQHandler(){
	uint32_t flags= (I2C1->ISR);
	if(flags & (1UL<<8)){GPIOC->ODR|=1UL<<8;} //*Visual Feedback for Bus Error*//
	if(flags & (1UL<<9)){GPIOC->ODR|=1UL<<11;} //*Visual Feedback for Arbitration Loss*//
	if(flags & (1UL<<10)){GPIOC->ODR|=1UL<<10;} //*Visual Feedback for Overrun Error*//
	I2C1->ICR|=7UL<<8; //*Clearing all three error flags*//
}
void DMA1_Channel6_IRQHandler(){
    uint32_t flags= (DMA1->ISR);
	DMA1->IFCR|=0xFU<<20; //*Clearing DMA1 Channel 6 Flags*//
	I2C1->CR1&=~(1UL<<14); //*Disabling TXDMAEN after each transfer to properly end the sequence*//
	DMA1_Channel6->CCR&= ~1UL; //*Disabling DMA1 Channel 6 after each transfer*//
	if(flags & (1UL<<23)){GPIOC->ODR|=1UL<<12;} //*Visual Feedback for Channel 6's Transfer Error*//
}
void DMA1_Channel7_IRQHandler(){
    uint32_t flags= (DMA1->ISR); 
	DMA1->IFCR|=0xFU<<24; //*Clearing DMA1 Channel 7 Flags*//
	if(flags & (1UL<<25)){ //*Checking for Transfer Complete Interrupt Flag*//
	  I2C1->CR1&=~(1UL<<15); //*Disabling RXDMAEN after each reception to properly end the sequence*//
	  DMA1_Channel7->CCR&= ~1UL; //*Disabling DMA1 Channel 7 after each transfer*//
	  if(flags & (1UL<<27)){GPIOC->ODR|=1UL<<9;} //*Visual Feedback for Channel 7's Transfer Error*//
    }
}	
