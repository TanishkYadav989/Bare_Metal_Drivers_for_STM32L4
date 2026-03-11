This is a Pure Register-Level (Bare Metal) programming repository for the STM32 L476RG micro-controller.
This repository contains drivers for each & every major peripherals of this controller with different approach such as Polling based, Interrupt based & DMA integrated.
The Drivers are written from scratch in C Language using Raw Bit Manipulation & CMSIS Header file which helps in decreasing the layered structures of High Level Libraries (HAL,etc.) & reducing CPU overhead & consumption by directly communicating to the Hardware.
Every Driver in this repository has been implemented in real life on breadboard including all the necessary eqiupments for this.
For High level debugging and tracking the behaviour of each peripheral closely, I have also used an 8-Channel Logic Analyzer. Captures are attached for each respective driver.
Software Used:
1. Keil uvision -> This is the primary compiler, used for its safety critical features for Raw C & according to the Industrial Standard.
2. VS Code      -> This is mainly used as an editor to create Header/Source Files & commenting/documenting each driver.
3. Pulse View   -> Used to Capture the raw electrical signals and decoder signals from the pins of the Board.
4. Terraterm    -> Used to Transmit Values and Information from the board to PC for proper debugging. Used of UART protocol.

List of Drivers:
1. Lighting Up an LED using GPIO
2. Increasing Speed of Toggling of LED/Buzzer using EXTI & SysTick
3. Breathing LED / Buzzer Siren using PWM with Timers
4. Transmitting Data to PC's TerraTerm Terminal using UART based on Polling, Interrupt & DMA
5. Reading data from MPU6050 sensor (Accelrometer & Gyro) using I2C protocol based on Polling & DMA
6. SPI Loopback Driver by interconnecting MISO & MOSI based on Interrupt & DMA
7. Reading data with a 10 Kohm potentiometer using ADC based on DMA & Interrupt
   
