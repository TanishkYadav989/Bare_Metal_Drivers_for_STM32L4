This is a Pure Register-Level (Bare Metal) programming respository for the STM32 L476RG micro-controller.
This repository contains drivers for each & every major peripherals of this controller with different approach such as Polling based, Interrupt based & DMA integrated.
The Drivers are written from scratch in C Language using Raw Bit Manipulation & CMSIS Header file which helps in decreasing the layered structures of High Level Libraries (HAL,etc.) & reducing CPU overhead & consumption by directly communicating to the Hardware.
Every Driver in this repository has been implemented in real life on breadboard including all the necessary eqiupments for this.
For High level debugging and tracking the behaviour of each peripheral closely, I have also used an 8-Channel Logic Analyzer. Captures are attached for each respective driver.
Software Used:
1. Keil uvision -> This is the primary compiler, used for its safety critical features for Raw C & according to the Industrial Standard.
2. VS Code      -> This is mainly used as an editor to create Header/Source Files & commenting/documenting each driver.
3. Pulse View   -> Used to Capture the raw electrical signals and decoder signals from the pins of the Board.
4. Terraterm    -> Used to Trasmit Values and Information from the board to PC for proper debugging. Used of UART protocol.
  
