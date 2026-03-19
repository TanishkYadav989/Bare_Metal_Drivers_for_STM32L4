#include "stm32l476xx.h"
#include "SPI_DMA.h"
#include "UART.h"
void SystemInit(void){}
int main(){
    volatile uint8_t tx[4]={0x00,0xAA,0xFF,0x55U};
    volatile uint8_t rx[4];
    char str[30];
    GPIOinit_SDMA();
    gpioinit_UART();
    UART_init(USART2,0x341);
    GPIO_Status();
    SPI_DMA_init(SPI1);
    SDMA_init();
    CS_H();
    SDMA_WR(tx,rx,4);
    while(1){
      sprint(str,"%X %X %X",tx[0],tx[1],tx[2]);
      UART_msg(USART2,(uint8_t *)tx);
    }
}
