#ifndef I2Cx_H
#define I2Cx_H
#include "stm32l476xx.h"
#include <stdint.h>
void GPIO_init_i2c();
void I2C_init(I2C_TypeDef* I2Cx, uint32_t ownadd);
void Start(I2C_TypeDef* I2Cx, uint32_t device, uint8_t size, uint8_t dir);
void Send_I2C(I2C_TypeDef* I2Cx, uint8_t slave, uint8_t *data, uint8_t size);
void Receive_I2C(I2C_TypeDef* I2Cx, uint8_t slave, uint8_t *data, uint8_t size);
void I2C1_EV_IRQHandler();
void MPU_send(uint8_t reg, uint8_t data);
void MPU_receive(uint8_t reg, uint8_t *value, uint8_t size);
#endif 