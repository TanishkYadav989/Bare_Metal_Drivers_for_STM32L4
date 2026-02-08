#include "stm32l476xx.h"
#include "gpio.h"
void SystemInit(){
}
int main(){
  LED_ACTIVATE();
	ENABLE_LED();
  while(1);  
}