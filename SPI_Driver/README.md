This driver uses the Serial Peripheral Interface (SPI) which is a Full-Duplex serial communication protocol & used for High Speed Data Transfer upto 50 MBps or more depending upon the OSPEEDR config for the data pins.
To test my Configuration & knowledge of SPI, I have implemented a Loopback circuit by interconnecting MISO & MOSI lines & verified its signal integrity & transfer using a Logic Analyzer.

SPI is usually used for OLED displays (displays that requires high refresh rate) & High Speed External Storage Devices.

PA5, PA6, PA7 are mapped as Alternate Functions for (Serial Clock), MISO line (Master-In/Slave-Out), MOSI line (Master-Out/Slave-In) respectively & PB6 is used as a CS line (Chip Select).
Since SPI is not a Bus unlike the I2C, the Slave devices are connected to normal GPIO pins in High State & selected when those pins are held Low.
For Configuring the SPI we enable its clock & reset the whole peripheral to remoev any Stale bits.
The Bi-directional mode is disabled & the Baud rate (transfer speed) is set to Processor Clock divided by 64 i.e 62.5 KHz for a 4MHz clock.
Slave Selection Management (SSM) & Slave Selection Internal (SSI) is enabled for Slave selection manually.
Data Size is set to 8-bit with MSB first mode & FIFO Reception Threshold bit (FRXTH) is set to correctly align the 8-bit data.
Clock Phase & Clock Polarity both are set, to capture on the Rising edge of the Clock. The Controller is set as Master to control the Conversation.
Finally SPI is enabled along with its Interrupt via NVIC.

As it is a synchronus full duplex SCP, a single read & write function is initialised in which we map the function argument buffers to the Globally initialised Buffers & variables for index & size. After this TXEIE & RXEIE is set to start the Conversation.
In the ISR, we check for the RXNE & TXE flag and write/read the data manually to the global buffers accordingly.
When Reception is complete CS pin is turned high again to De-select the Slave & end the conversation. A ready flag is set to check is the Transfer is completed.

In the main.c file, the CS is turned high first as default & then before starting the Conversation the CS pin is held low to Select the Slave.
The Logic Analyzer Capture & the Terminal Data Capture both are attached for the same.
