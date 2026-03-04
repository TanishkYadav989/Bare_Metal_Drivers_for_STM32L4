#include "stm32l476xx.h"
#include "SPIx.h"
#include "UART.h"
#include "SysTick.h"
#include <stdio.h>
extern volatile uint8_t ready;             //*Data ready flag*//
void SystemInit(void){
}
int main(){
    volatile uint8_t msg[]={0x01,0x02,0x03,0x00};    
    volatile uint8_t rev[sizeof(msg)];
    char buff[20];
    GPIO_config();
    systick_int(4000);
    SPI_init(SPI1);
	gpioinit_UART();
	UART_init(USART2,0x341);
	while(1){
    CS_L();                                  //*Pulling PB6 low to select the pin*//
		delay(100);
		SPI_WR(SPI1,msg,rev,sizeof(msg));    //*Sending Hex values & receive at the same time for Loopback circuit*//
		delay(1000);
		if(ready==1){
		  sprintf(buff,"0x%02X 0x%02X 0x%02X\r\n",rev[0],rev[1],rev[2]);  //*Retargetting sprintf to work with UART & read received Data*//
		  UART_msg(USART2,(volatile uint8_t*)buff);
		}
    }	
}