#ifndef EXTI_9
#define EXTI_9
#include "stm32l476xx.h"
extern volatile int counter;
extern volatile int button;
void button_init();
void exti_init();
void EXTI9_5_IRQHandler();
#endif
