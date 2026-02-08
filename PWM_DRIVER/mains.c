#include "stm32l476xx.h"
#include "timer.h"
void SystemInit(){
}
int main(){
  gpioinit();
  timer3init(7, 300, 0);     //*Prescaler=7 , Auto Reload Value=300 , Counter=0*//
  volatile uint16_t brightness;
  while(1){
    for(brightness=0;brightness<300;brightness++){            //*Loop for Scaling up the brightness i.e increasing counter value*//
      TIM3->CCR3=brightness;                      
      for(volatile int i=0;i<5000;i++);                          //*Short delay for signal settling*//
    }
    for(brightness=300;brightness>0;brightness--){
      TIM3->CCR3=brightness;;
      for(volatile int i=0;i<5000;i++);
    }
  }
}