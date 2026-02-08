#include "stm32l476xx.h"
#include "usart.h"
void SystemInit(){
}
int main(){
	uint8_t a[]="Hey, UART is working!\r\n";
	gpioinit_usart();
	usartinit(USART2,0x45);	
  while(1){
	USART_Write(USART2, a, sizeof(a)-1);		 
  }	
}