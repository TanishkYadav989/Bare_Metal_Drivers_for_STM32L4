#ifndef ADC_H
#define ADC_H
#include "stm32l476xx.h"
void GPIOinit_ADC();
void Timer4_init(uint16_t psc, uint16_t arr);
void ADC1_init(uint16_t size, volatile uint16_t *buffer);
void ADC1_2_IRQHandler();
void DMA1_Channel1_IRQHandler();
#endif
