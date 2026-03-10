#include "stm32l476xx.h"
#include "I2C_DMA.h"
#include "UART.h"
#include "stdio.h"
void SystemInit(void){
}
extern volatile int dready;
extern uint8_t r[14];
int main(){
	static uint8_t buff[2]={0x6B,0x00};
	uint8_t s[34];
	GPIO_init_IDMA();
	GPIO_Istatus();
	gpioinit_UART();
	UART_init(USART2,0x341);
	I2C_DMA_init(I2C1,0x53);
	IDMA_init();
	I2C_transmit(buff,2,0x68);
	while(1){
	 if(dready==1){	
		 dready=0;
		int16_t AX=((r[0]<<8)|r[1]);
		int16_t AY=((r[2]<<8)|r[3]);
		int16_t AZ=((r[4]<<8)|r[5]);
		sprintf((char *)s,"AX=%d AY=%d AZ=%d\r\n",AX,AY,AZ);
		UART_msg(USART2,(volatile uint8_t *)s);}
	}
}