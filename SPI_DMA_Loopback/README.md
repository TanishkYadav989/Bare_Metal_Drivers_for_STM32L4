*In the terminal capture we can see that the data is shifted towards right. This is a byte offset that usually occurs when use loopback system with automated data transfer with DMA. It is an Errata for the STM32L4 series & won't occur in generalised usage*

This driver is the Integration of SPI a High-Speed, Full-Duplex data transfer peripheral with Automated Data transfer using the DMA Engine for lesser CPU overhead & preventing manual data read & writes. This combination is very useful for transferring & receiving large chunks of data with very high speed suitable for OLEDs & External Storage devices. This DMA approach full eliminates the polling overhead entirely & enables simultaneous processing necessary for real-time systems.

GPIO Pins used:
1. PA5, PA6 & PA7 Alternate Functions for SCK, MISO & MOSI lines.
2. PB6 is used for Chip Select line for a Slave.

SPI Configuration :
1. Enabling Clock using RCC's APB2 to power up SPI1 & completely resetting the peripheral using RCC's RSTR.
2. Setting Slave Selection Management & Internal bit for manual slave selection.
3. Setting Baud Rate for Fclk/256 & enabled the error interrupt to detect errors via an LED.
4. Using SPI mode 3 i.e both CPHA & CPOL being set to capture at rising edge.
5. Data size is set to 8-bit & enabling FIFO reception threshold (FRXTH) for proper data alignment of 8-bit data in the 16-bit DR.
6. Enabling its peripheral interrupt using NVIC.

DMA config & READ/WRITE function :
1. Enabling Clock for DMA1 using RCC's AHB1 & configuring channel 2 & 3 for 8-bit PSIZE/MSIZE, later setting their priority.
2. Setting them for memory increment to read/write value from/to fill the whole buffer.
3. As Channel 2 is mapped to Rx, it is set to read from peripheral & channel 3 is mapped to Tx, it is set to read from memory.
4. Setting Transfer Complete & Transfer Error interrupt & the enabling the SPI1 interrupt using NVIC.

5. The WR function follows the exact sequence given in the Reference Manual (RM0351) to initiate SPI transfer & reception using DMA. 
6. Clearing all the DMA1 flags for un-interrupted data transfer & CS pin is pulled low for Slave Selection & transmissions/receptions from that pin.
7. Assigning the Memory & Peripheral's Data register address to CMAR & CPAR for both channels.
8. The CNDTR is assigned the data size for stream termination when the transfer is completed.
9. The first byte is manually sent to trigger the Transfer & Lastly enabling the SPI1 peripheral to Start transmission.

Interrupt Service Routines :
1. The SPI1's ISR only checks for the OVERRUN error and enables an LED for Visual Feedback & error detection.
2. In DMA CH2's ISR, the DMA Channel 2 which is for Rx checks for the Transfer Complete Flag & inside it we check the if the SPI is busy or not. If the SPI is not busy then this indicates that the Reception is done & the transfer is completed. Then the complete SPI DMA disabling sequence is initiated to safely end the whole SPI_DMA's Data Stream as mentioned in the Reference Manual. Pulling the CS pin high again to De-Select the Transmission/Reception line. The TE flag is also set to detect error using an LED.
3. The DMA CH3's ISR is same as the SPI1's, it only checks for the TE flag & gives a visual feedback of the error using the LED.

The Main file one defines the Buffers for tx & rx and call these functions in the correct sequence & prints the received data using UART.
The Terminal Data Capture is attached for the Same.
