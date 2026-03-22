#include "ADC.h"
#include "timer.h"
#include "exti.h"
#include "stm32l476xx.h"
#include <stdbool.h>
extern volatile bool accept;                                        //*Defined in exti.c for button press detection*//
volatile bool thresh = false;                                       //*Defined to Check if the threshold value is reached/crossed*//
volatile uint16_t *BUFF;                                            //*Global Buffer to access ADC values in other files*//
volatile int16_t duty;                                              //*Global variable to access the modulating PWM duty cycle in other files*//
extern volatile uint16_t areload;                                   //*ARR value of TIM3 accessed from timer.h*//
void GPIOinit_ADC(){
    RCC->AHB2ENR|=1UL;                                               //*Enabling Clock of GPIO Port A*//
    GPIOA->MODER&=~(0x3U);                                           
    GPIOA->MODER|=(0x3U);                                            //*Setting PA0 to Analog Input Mode (11)*//
    GPIOA->ASCR|=1UL;                                                //*Enabling Analog switch to ADC input via ASCR*//
}
void Timer4_init(uint16_t psc, uint16_t arr){
    RCC->APB1ENR1|=1UL<<2;                                           //*Enabling Clock of Timer 4*//
    TIM4->CR1=0;                                                     //*Clearing CR1 of TIM4*//
    TIM4->CR2=0;                                                     //*Clearing CR2 of TIM4*//
    TIM4->CR1&=~1UL;                                                 //*Disabling TIM4*//
    TIM4->CR2&=~((0x7U)<<4);                                         
    TIM4->CR2|=(1UL<<5);                                             //*Setting Timer 4 for update event with TRGO*//
    TIM4->PSC= psc;                                                  //*Assigning Prescaler using Function Argument*//
    TIM4->ARR= arr;                                                  //*Assigning Auto-Reload value via Function Argument*//
    TIM4->CR1|=1UL;                                                  //*Enabling Timer 4 to trigger ADC output*//
}
void ADC1_init(uint16_t size, volatile uint16_t *buffer){
	RCC->CCIPR|=3UL<<28;                                             //*Setting System Clock i.e 4MHz selected as ADC's Clock*//
    RCC->AHB2ENR|=1UL<<13;                                           //*Enabling Clock for ADC (AHB Bus-2)*//
    RCC->AHB1ENR|=1UL;                                               //*Enabling Clock for DMA1 (AHB Bus-1)*//
    RCC->AHB2RSTR|=1UL<<13;
    RCC->AHB2RSTR&=~(1UL<<13);                                       //*Completing Reset of ADC to discard Stale Bits*//

    ADC123_COMMON->CCR=0;                                            //*Resetting ADC's Common register for setting psc to 0 & Asynchronous Clock Mode*//
    ADC1->CR=0;                                                      //*Atomic clearing of CR & Waking ADC from Deep-Power-down mode*//
    ADC1->CR|=1UL<<28;                                               //*Enabling ADC Voltage Regulator*//
    for(volatile int i =0; i<10000;i++);                             //*Waiting to successfully enable ADC VREG to prevent race conditions for it, as suggested in RM*//
    ADC1->CR|=1UL<<31;                                               //*Calibrating ADC*//
    while(ADC1->CR & (1UL<<31));                                     //*Waiting until Calibrating ADC*//

    ADC1->CFGR=0;                                                    //*Atomic clearing Configuration Register of ADC1*//
	ADC1->CFGR|=1UL<<1;                                              //*Enabling DMA circular mode*//
	ADC1->CFGR|=1UL<<23;                                             //*AWD1 enabled for detecting the 75% threshold*//
	ADC1->CFGR|=1UL<<22;                                             //*AWD1 enabled for single channel*//
	ADC1->CFGR|=(0x5U)<<26;                                          //*Mapping ADC1 Channel 5 for Analog Watchdog 1*//
	ADC1->TR1=0;
	ADC1->TR1|= ((3068U<<16));                                       //*Setting the High-Threshold Value to 3068 (75% of 4090) & Low Threshold Value to 0*//
	ADC1->CFGR|=(12UL<<6);                                           //*Mapping Timer 4 TRGO (0110) to automatically trigger the conversion*//
	ADC1->CFGR|=1UL<<10;                                             //*Setting Hardware trigger detection for rising edge*//
	ADC1->CFGR|=1UL;                                                 //*Enabling DMA data transmission*//
    ADC1->SMPR1|=3UL<<16;                                            //*Setting Sampling Rate to 247.5 ADC Clock Cycles*//
    ADC1->SQR1|=(0x5U)<<6;                                           //*1st Sequence mapped to ADC1 Channel 5*// 
	ADC1->IER|=1UL<<7;                                               //*Enabling Analog Watchdog 1's interrupt//
    
    DMA1_Channel1->CCR=0;                                            //*Clearing DMA1 Channel 1's Control Register*//
    BUFF=buffer;                                                     //*BUFF pointing to the same address as buffer*//
    DMA1_Channel1->CCR|=(0x5U)<<8;                                   //*Setting Peripheral & Memory size to 16 bit*//
    DMA1_Channel1->CCR|=3UL<<12;                                     //*Setting CH1's priority to highest*//
    DMA1_Channel1->CCR|=1UL<<7;                                      //*Enabling Memory increment mode*//
    DMA1_Channel1->CCR|=(0xAU);                                      //*Enabling TC & TE interrupts*//
    DMA1_Channel1->CCR|=1UL<<5;                                      //*Enabling Circular mode*//
    DMA1_Channel1->CPAR= (uint32_t)&(ADC1->DR);                      //*Allocating the address of the ADC1's regular channel Data register as the source of transfer*//
    DMA1_Channel1->CMAR= (uint32_t)BUFF;                             //*Allocating the adrress of BUFF as the destination of the transfer*//
    DMA1_Channel1->CNDTR= size;                                      //*Assigning the size of buffer argument*//
    DMA1_CSELR->CSELR&=~(0xFU);                                      //*Mapping ADC1 to DMA1 Channel 1 via Select Register*//
    NVIC->IPR[11]=0x20;                                              
    NVIC->ISER[0]|=1UL<<11;                                          //*Enabling DMA1 CH1 interrupt to access its Handler*//
    DMA1_Channel1->CCR|=1UL;                                         //*Arming DMA1 Channel 1 before enabling ADC1 *//
	ADC1->CR|=1UL;                                                   //*Enabling ADC1*//
	NVIC->IPR[18]=0x00;                                              //*Highest priority of ADC1_2 interrupt*//
	NVIC->ISER[0]|=1UL<<18;                                          //*Enabling ADC1_2 interrupt*//
	while(!(ADC1->ISR & (1UL)));                                     //*Polling until ADC ready flag is set in the ISR*//
    
    ADC1->CR|=1UL<<2;                                                //*Setting ADC start bit to start the conversion after ADC1 is ready*//
}
void ADC1_2_IRQHandler(){
	if(ADC1->ISR & (1UL<<7)){                                        //*Checking for the AWD1 flag*//
	    if(BUFF[0]>3068){                                            //*Checking if the Threshold is crossed*//
			thresh=true;                                             //*Thresh is set to TRUE*//
			TIM3->CCR3=0;                                            //*LED is disabled*//
			GPIOC->ODR|=1UL;                                         //*Buzzer is enabled as warning*//
			ADC1->ISR|=1UL<<7;                                       //*CLearing the AWD1 flag*//
		}
		else{
			thresh=false;                                           //*If the Threshold is not crossed, thresh is FALSE & Buzzer is de-activated*//
			GPIOC->ODR&= ~(1UL);
			ADC1->ISR|=1UL<<7;                                      //*AWD1 flag is cleared*//
		}
	}
}
void DMA1_Channel1_IRQHandler(){
    if(DMA1->ISR & (1UL<<1)){                                       //*Checking if DMA1 CH1's transfer is completed*//
		if(thresh && (BUFF[0]<3068 || accept)){                     //*Checking if threshold is crossed & ADC value below threshold or button press event has occuured*//
            thresh=false;                                           //*Thresh is set back to FALSE again*//
		    accept=false;                                           //*Button event is set to 0 again*//
			GPIOC->ODR&=~1UL;                                       //*Buzzer is De-activated*//
			ADC1->TR1=((0xFFFU)<<16);                               //*High Threshold limit is set to max, so the buzzer will remain off & LED will remain on after button press*//
			}
		if(BUFF[0]<3068){
			ADC1->TR1=(3068U<<16);                                  //*If ADC value gets below the assigned threshold again, the old threshold limit is set again for warning & override (if pressed) again*//
			ADC1->ISR|=1UL<<7;                                      //*Re-Arming the Analog Watchdog by clearing the AWD1 flag*//
		}
		if(!thresh){
            TIM3->CCR3=(BUFF[0]*areload)/4095;                      //*If threshold is not reached, the potentiometer knob can still manipulate the duty cycle of the PWM LED*//
		    duty= TIM3->CCR3;                                       //*The value of the Duty Cycle is assigned to 'duty' variable for terminal reading*//
		}	
        DMA1->IFCR|=(0xFU);                                         //*Clearing Interrupt Flags for DMA1 CH1*//
 }
    if(DMA1->ISR & (1UL<<3)){                                       //*If transfer error is occured, the flags are cleared*//
        DMA1->IFCR|=(0xFU);
    }
}