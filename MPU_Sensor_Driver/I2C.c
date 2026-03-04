#include "I2Cx.h"
#include "stm32l476xx.h"
#include <stdint.h>
volatile uint8_t *data_tx=0;
volatile uint8_t size_tx=0;
volatile uint8_t index=0;
volatile uint8_t *data_rx=0;
volatile uint8_t size_rx=0;
volatile uint8_t index_r=0;
void GPIO_init_i2c(){
    RCC->AHB2ENR|=1UL<<1;                               //*Enabling clock of GPIO Port B*// 
    GPIOB->MODER&= ~((0xFU)<<16);                     
    GPIOB->MODER|=(0xAU)<<16;                           //*Setting pin 8 & 9 for Alternate Function (10)*//
    GPIOB->OTYPER&= ~(3UL<<8);                          
    GPIOB->OTYPER|=3UL<<8;                              //*Setting Output Type as Open Drain which is required by SCL & SDA*// 
    GPIOB->PUPDR&= ~((0xFU)<<16);                       //*Disabling pull up as the module already has in-built Pull up resistors*//
    GPIOB->AFR[1]&= ~(0xFFU);                           
    GPIOB->AFR[1]|=(0x44U);                             //*Enabling AF4 for I2C SCL & SDA for pin 8 & 9 using AFRH*//
}
void I2C_init(I2C_TypeDef* I2Cx, uint32_t ownadd){
    RCC->APB1ENR1|=1UL<<21;                             //*Enabling clock for I2C1*//
    RCC->APB1RSTR1|=1UL<<21;                            //*Setting reset register to clear out any garbage bits*//
    RCC->APB1RSTR1&= ~(1UL<<21);                        //*Clearing to disable reset register*//

    I2Cx->CR1&= ~(1UL);                                 //*Disabling I2C1 first*//
    I2Cx->CR1&= ~((0xFU)<<8);                           //*Disabling Digital Noise Filter*//
    I2Cx->CR1&= ~(1UL<<12);                             //*Enabling Analog Filter*//
    I2Cx->CR1&= ~(1UL<<17);                             //*Clock Stretching Enabled*// 
    I2Cx->CR1&= ~(3UL<<20);                             //*Disabling SMBus*//
    I2Cx->CR1|= (3UL<<1)|((0xFU)<<4);                   //*Enabling RXEIE, TXEIE, NACKIE, STOPIE, TCIE & ERRIE bits*//
    
    I2Cx->TIMINGR=0;                                    //*Clearing Timing Register*//
    I2Cx->TIMINGR&= ~(0xFU<<28);                        //*Clearing last 4 bits to set the Presclaer to 0*//
    I2Cx->TIMINGR|= ((0x13U) | (0x11U)<<8 | (0x2U)<<16 | (0x4U)<<20);             //*SCLL= 13, SCLH= 11,SDADEL= 2, SCLDEL= 4 according to the minimum requirements for the sensor*//

    I2Cx->OAR1&= ~(1UL<<10);                            //*Setting Own Address as 7-bit*//
    I2Cx->OAR1|= (ownadd);                              //*Providing own address via function argument*//
    I2Cx->OAR1&= ~(1UL<<15);                            //Own Address 1 is disabled. The received address OA1 is NACKed*//

    I2Cx->CR2&= ~(1UL<<11);                             //*Disabling for 10 bit Master Addressing Mode*//
    I2Cx->CR2|=(1UL<<15);                               //*Enabling NACK, sent after current received byte*//                
    I2Cx->CR1|=1UL;                                     //*Enabling I2C1*//
    NVIC->IPR[7]=0x10;                                  //*Setting Interrupt Priority high*//
    NVIC->ISER[0]|=1UL<<31;                             //*Enabling Interrupt*//
		for(volatile int i=0;i<1000;i++);               
}
void Start(I2C_TypeDef* I2Cx, uint32_t device, uint8_t size, uint8_t dir){
    uint32_t REG = I2Cx->CR2;                           //*Initialising a temporary register to configure CR2 bits all together*// 
    REG &= (uint32_t)~(uint32_t)(I2C_CR2_SADD |(1UL<<10)|(3UL<<24)|((0xFFU)<<16)|(3UL<<13));    //*Clearing bits to ensure safety*//
    if(dir==1){
        REG|=1UL<<10;                                   //*If dir is 1, the master requests for a Read transfer otherwise Write transfer*//
    }
    REG|= (((uint32_t)(device & I2C_CR2_SADD)) | (uint32_t)(size<<16 & ((0xFFU)<<16)));    //*Setting Slave Address and the data bytes required using function arguments*//
	REG|=1UL<<25;                                       //*Enabling Autoend to automatically send a stop bit after data transfer is done*//
    REG|=1UL<<13;                                       //Enabling the Start bit to generate a Start Condition*//
    I2Cx->CR2 = REG;                                    //*Finally allotting all the values back to CR2*//
}
void Send_I2C(I2C_TypeDef* I2Cx, uint8_t slave, uint8_t *data, uint8_t size){
    if(I2Cx->ISR & (1UL<<15))                           //*Checks for Busy Flag in the ISR to see if the Wait line is idle*//
      return;                                           //*If Empty returns the function*//
    data_tx=data;                                       
    size_tx=size;
    index=0;
    Start(I2Cx,slave,size,0);                           //*Start bit is initiated with 0 representing a Write bit*//
}
void Receive_I2C(I2C_TypeDef* I2Cx, uint8_t slave, uint8_t *data, uint8_t size){
    if(size==0 || data==0)                              //*If data buffer is empty or data size is 0, fuction returns*//
      return;
    if(I2Cx->ISR & (1UL<<15)){                          //*Checks for Busy Flag in the ISR to see if the Wait line is idle*//
        return;
    }
    data_rx=data;
    size_rx=size;
    index_r=0;    
    Start(I2Cx,slave,size,1);                           //*Start bit is initiated with 1 representing a Read bit*//
}
void I2C1_EV_IRQHandler(){
    if(I2C1->ISR & (1UL<<1)){                          //*Checks for the TXE flag to check if the Transmit Data register is empty*//
        if(index<size_tx){
           I2C1->TXDR= data_tx[index++];               //*Writes Data to TXDR register for Transmission*//
        }
    }
    if(I2C1->ISR & (1UL<<2)){                         //*Checks for the RXNE flag to check if the Receive Data register is n ot empty*//
        if(index_r<size_rx){
          data_rx[index_r++]= I2C1->RXDR & 0xFF;      //*Reads data into the Globally initialised buffer to receive readings*//
        }   
    }
    if(I2C1->ISR & (1UL<<5)){                        //*Checks for the STOP bit sent Flag*//
        I2C1->ICR|=1UL<<5;                           //*Setting STOPF bit in ICR to clear the flag*//
        data_tx=0;                                   //*Resets Data buffer*//
        data_rx=0;                                   //*Resets Receive buffer*//
    }    
	if(I2C1->ISR & (1UL<<4)){                        //*Checks for NACK Flag, if NACK occured or not*//
		I2C1->ICR|=1UL<<4;                           //*Clearing Nack flag from ICR*//
        I2C1->CR2|=1UL<<14;                          //*Manually sending a STOP bit*//
	}
}
void MPU_send(uint8_t reg,uint8_t data){            //This fuction is initialised to Send the instructions to the MPU-6050 Sensor*//
    uint8_t inst[2]={reg,data};                     //*Sends the register address and its instructions in form of an Array*// 
    Send_I2C(I2C1,0xD0,inst,2);                     //*Initialising a Start condition to send the Array to the Sensor*//
    while(data_tx!=0);                              //*Polling until data transmission buffer is reset*//
}
void MPU_receive(uint8_t reg, uint8_t *value, uint8_t size){       //*This function is to read data from the Sensor*//
	  Send_I2C(I2C1,0xD0,&reg,1);                                  //*Sends to Read the particular register in which the data of the sensor is stored*//
	  while(data_tx!=0);                                           //*Polling until data transmission buffer is reset*//
	  Receive_I2C(I2C1,0xD0,value,size);                           //*Receiving data from the data registers*//
	  while(data_rx!=0);                                           //*Polling until receive buffer is cleared*//
}