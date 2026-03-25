#include "exti.h"
#include "stm32l476xx.h"
#include "ADC.h"
#include <stdbool.h>
extern volatile bool thresh; //*Defined in ADC.h to check if the threshold is reached/exceeded*//
volatile bool accept=false; //*Defined for the override status*//
void button_init(){
    RCC->AHB2ENR|=RCC_AHB2ENR_GPIOBEN; //*GPIO Port B CLock Enable*//
    GPIOB->MODER&=~(3UL<<18); //*Clearing bits of Pin 9, Input Mode*//
    GPIOB->PUPDR&=~(3UL<<18);               
    GPIOB->PUPDR|=1UL<<18; //*Setting bit 1 of Pin 9, Internal Pull up Mode*//
}
void exti_init(){
    RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN; //*SYSCFG Clock Enable of External Interrupt*//
    SYSCFG->EXTICR[2]&=~(15UL<<4);         
    SYSCFG->EXTICR[2]|=1UL<<4; //*Setting EXTI_9 in EXTICR2 Register*//

    EXTI->RTSR1|=1UL<<9; //*Disable Rising Trigger Register*//
    EXTI->FTSR1&=~(1UL<<9); //*Enabling Falling Trigger Register for detecting Button Release*//
    EXTI->IMR1|=1UL<<9; //*Enabling Interrupt Mask Register-1 for EXTI_9*//
    NVIC->IPR[7]=0x20; //*Setting Interrupt Priority (High)*//
    NVIC->ISER[0]|=1UL<<23; //*Enabling EXTI_9 Interrupt from Vector Table*//
}
void EXTI9_5_IRQHandler(){
    if((EXTI->PR1 & (1UL<<9))){
	   for(volatile int i=0;i<10000;i++); //*Short Delay for Signal Settling*//
	   if(thresh){ //*Checking if the threshold of 75% is exceeded when the button is pressed*//
			accept=true; //*The button press will override the warning & the LED will be back online*//
		}
     EXTI->PR1|=1UL<<9; //*Clearing Interrupt Flag by Setting it*//
    }
}
