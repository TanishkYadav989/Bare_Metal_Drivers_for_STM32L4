#ifndef SYSTICK_H
#define SYSTICK_H
#include "stm32l476xx.h"
extern volatile uint16_t t;
void systick_int(uint16_t ticks);
void SysTick_Handler();
void delay(uint16_t time);
#endif