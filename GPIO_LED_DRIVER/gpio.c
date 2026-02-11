#include "gpio.h"
#include "stm32l476xx.h"
void LED_ACTIVATE(void){
	RCC->AHB2ENR|=RCC_AHB2ENR_GPIOCEN;    //*Clock Enable for GPIO Port B*//
	GPIOC->MODER&=~3UL;                   //*Clearing Bits in PC0 for garbage values*//
	GPIOC->MODER|=1UL;                    //*Setting PC0 for GP Output Mode*//
	GPIOC->OTYPER&=~1UL;                  //*Setting Output Type as Push-Pull*//
	GPIOC->PUPDR&=~3UL;                   //*Clearing PC0 for no pull up/no pull down*//
	GPIOC->OSPEEDR&=~3UL;                 //*Clearing PC0 for Low Speed Output*//
}
void TOGGLE_LED(void){
	GPIOC->ODR^=1UL;                      //*Toggling bit of PC0 Output*//
}
void ENABLE_LED(void){ 
	GPIOC->ODR|=1UL;                     //*Setting bit of PC0 for Output*//

}
