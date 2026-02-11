#include "SysTick.h"
#include "stm32l476xx.h"
volatile uint16_t t;
void systick_int(uint16_t ticks){
    SysTick->CTRL=0;                    //*Disabling Control Register*//
    SysTick->LOAD=ticks-1;              //*Frequency Setting Value and -1 for starting From 0*//
    SysTick->VAL=0;                     //*Setting Counter Value*//
    SCB->SHPR[11]=0xF0;                 //*Setting Interrupt priority to lowest, -11th in the Vector Table*//
    SysTick->CTRL=7UL;                  //*Setting Control & Status Register (CLKSOURCE, TICK INT, ENABLE)*//
}
void SysTick_Handler(){                 //*SysTick Interrupt Handler*//
	if(t>0)                             //*Counts down according to the Delay provided*//
		t--;                           
}
void delay(uint16_t time){
  t=time;
	while(t!=0);

}
