#include "timer.h"
#include "stm32l476xx.h"
void gpioinit(){
    RCC->AHB2ENR|=RCC_AHB2ENR_GPIOBEN;          //*Enabling Clock For GPIO Port B*//
    GPIOB->MODER&=~3UL;                         
    GPIOB->MODER|=2UL;                          //*Setting PB0, Alternate Function Mode*//
	GPIOB->OTYPER&=~1UL;                        //*Setting Output for Push-Pull Mode*//
    GPIOB->AFR[0]&=15UL;                        
    GPIOB->AFR[0]|=2UL;                         //*Setting AFRL Pin 0 for Timer_3*//
}
void timer3init(uint16_t psc, uint16_t arr, uint16_t cnt){
    RCC->APB1ENR1|=1UL<<1;                     //*Enabling Clock For Timer_3*//
    TIM3->CR1&=~3UL<<5;                        
    TIM3->CR1|=3UL<<5;                         //*Setting Centre-Aligned Mode 3*//
    TIM3->PSC= psc;                            //*Setting Prescaler value*//
    TIM3->ARR= arr;                            //*Setting Auto Reload Register value*//
    TIM3->CCR3= cnt;                           //*Setting Counter value*//
    TIM3->CCMR2&=~TIM_CCMR2_OC3M;              
    TIM3->CCMR2|=TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;   //*Setting PWM Mode 1*//
    TIM3->CCER|=1UL<<8;                                 //*Capture/Compare_3 Output Enable*//
    TIM3->CR1|=1UL;                                     //*Enable Timer_3*//
}

