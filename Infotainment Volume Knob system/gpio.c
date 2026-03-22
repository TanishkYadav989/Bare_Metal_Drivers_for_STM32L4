#include "gpio.h"
#include "stm32l476xx.h"
void BUZZ_ACTIVATE(void){
	RCC->AHB2ENR|=RCC_AHB2ENR_GPIOCEN;         //*Enabling Clock of GPIO Port C (AHB Bus-2)*//
	GPIOC->MODER&=~3UL;                              
	GPIOC->MODER|=1UL;                         //*Setting PC0 for General Purpose Output Mode (01)*//
	GPIOC->OTYPER&=~1UL;                       //*Output is set as push pull*//
	GPIOC->PUPDR&=~3UL;                        //*No pull up / pull down for the pin*// 
	GPIOC->OSPEEDR&=~3UL;                      //*Output speed is set to Low*//
}
void TOGGLE_BUZZ(void){
	GPIOC->ODR^=1UL;                           //*Toggling PC0*//                     
}