#include "stm32l476xx.h"
#include "UART.h"
#include "exti.h"
#include "gpio.h"
void SystemInit(){
}
int main(){
	static uint8_t a[]="button is working! :).\r\n";
    LED_ACTIVATE();
	ENABLE_LED();
	button_init();
	exti_init();
	gpioinit_UART();
	UART_init(USART2, 0x341);
  while(1){
		if(button==1){
		  UART_msg(USART2, a);
          button=0;			
		}
  }
} 