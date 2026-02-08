#include "stm32l476xx.h"
#include "systick.h"
#include "gpio.h"
void SystemInit(){
}
int main(){
  LED_ACTIVATE();
  systick_int(4000);       //*Setting System Timer to 4000Hz*//
  while(1){
    TOGGLE_LED();
    delay(1000);           //*Blinking led with a delay of 1 sec*//
  }
}