#include "stm32l476xx.h"
#include "ADC.h"
#include "UART.h"
#include "gpio.h"
#include "timer.h"
#include "exti.h"
extern volatile int16_t duty;          //*Imorting the Value of Duty Cycle from ADC.h*//
volatile uint16_t rev[1];              //*Receiver Buffer which will contain the ADC value*//        
void SystemInit(void){}
int main(){
	char c[30];                        
	BUZZ_ACTIVATE();                   
	button_init();                     
	exti_init();
	gpioinit();
    gpioinit_UART();
	UART_init(USART2,0x341);            //*Baud Rate set to 9600 i.e 0x341 for 4MHz Pclk*//
	GPIOinit_ADC();
	timer3init(3,999,999);              
	ADC1_init(1,rev);            
	Timer4_init(399,99);                //*For the TRGO the PSC is 399 & the ARR is 99*//
	while(1){
	  sprintf(c,"ADC VALUE :%u PWM DUTY:%d\r\n",rev[0],duty);
	  UART_msg(USART2,(uint8_t *)c);
	}
}