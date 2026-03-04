#include "stm32l476xx.h"
#include "UART_DMA.h"
void SystemInit(void){
}
int main(){
	uint8_t ch[]="I know how to use DMA!!\r\n";       //*Data to be Transmitted via UART*//
	GPIO_init_UDMA();   
	GPIO_status();
	UART_DMA_init(USART2,0x341);
	UDMA_init();
	UDMA_send(ch,sizeof(ch));                         //*Calling the Send func. of DMA to Transmit this Data from memory to USART's TDR*//
	while(1);
}