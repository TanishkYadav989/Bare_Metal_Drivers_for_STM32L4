#include "stm32l476xx.h"
#include "systick.h"
#include "gpio.h"
#include "exti.h"
void SystemInit(){
}
int main(){
  LED_ACTIVATE();
  button_init();
  exti_init();
  systick_int(4000);       //*Setting System Timer to 4000Hz*//
  while(1){
    switch(counter){       //*LED Blinking delay will decrease after Each Button Click*//
      case 0:
        delay(1000); break;
      case 1:
        delay(500); break;
      case 2:
        delay(250); break;
      case 3:
        delay(125); break;
      case 4:  
        delay(60); break;        
    }
  }
}