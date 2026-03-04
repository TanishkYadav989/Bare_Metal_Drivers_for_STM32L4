#include "stm32l476xx.h"
#include "I2Cx.h"
#include "UART.h"
#include "SysTick.h"
void SystemInit(void){
}
int main(){
	uint8_t r[50];
	char s[50];
	gpioinit_UART();
    UART_init(USART2,0x341);
    GPIO_init_i2c();
 	I2C_init(I2C1,0x53);
	MPU_send(0x6B,0x00);                                        //*Disabling the Sleep Mode of the Sensor in the PWR_MGMT1 register*//
	MPU_send(0x1C,0x08);                                        //*Configuring the Accelerometer for the Scale Range of 4g*//
	while(1){
		MPU_receive(0x3B,r,14);                                 //*Reading data from the data registers, after reading from 1st one it automatically reads the next one & increments so on*//
		int16_t AX = (int16_t)((r[0]<<8)|r[1]);                 //*Because there are two data registers for each axis, we combine their data via parsing to get one single value*//    
        int16_t AY = (int16_t)((r[2]<<8)|r[3]); 		
        int16_t AZ = (int16_t)((r[4]<<8)|r[5]); 		
        int16_t GX = (int16_t)((r[8]<<8)|r[9]); 		
        int16_t GY = (int16_t)((r[10]<<8)|r[11]); 		
        int16_t GZ = (int16_t)((r[12]<<8)|r[13]);
        sprintf(s,"AX=%d AY=%d AZ=%d GX=%d GY=%d GZ=%d\r\n",AX,AY,AZ,GX,GY,GZ);       //*Printing their respective values*
	    UART_msg(USART2, (volatile uint8_t*) s);
        delay(1000);		
	}
}