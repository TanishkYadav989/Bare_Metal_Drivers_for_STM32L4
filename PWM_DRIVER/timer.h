#ifndef TIMER_H
#define TIMER_H
#include "stm32l476xx.h"
void gpioinit();
void timer3init(uint16_t psc, uint16_t arr, uint16_t cnt);
#endif