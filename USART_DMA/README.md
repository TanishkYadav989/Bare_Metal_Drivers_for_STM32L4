*Using DMA results in the reduction of CPU overhead. The CPU is free all the time as the data transfer takes place in the background.*

1. This driver is the Integration of UART with DMA by replacing manual Data Register writes with automatic & fast data transfer using the On-board DMA Engine.

2. For enabling data requests from the UART to the DMA Engine, we have set the TXDMAEN bit in the CR3 register & disabling TXEIE.
Now we configure the DMA1 Channel 7's CCR for initiating data transfer bus for 8-bit in the UDMA_init function.
We allot the 32-bit Memory Address (buffer address) to CMAR register as the source of transfer & the Peripheral's Data Register Address to CPAR register as the destination of the transfer.
The size of the Buffer/Array is provided to CNDTR as a counter, which stops the data transfer when it hits 0.
When the data reaches the UART's Data Register, it automatically transmits it from the TX pin.

4. For this driver Normal Mode is used to transmit the buffer once. When we use circular mode, the buffer is continuously read which results in continuous transmission of it & also prevents buffer overflow.
The Interrupt Handler for DMA1 Channel 7 is used only to clear the Transfer Complete, Half Transfer Complete & Transfer Error flag.
LED Toggle is added in the Handler to get visual feedback of the status of the Transfer & error detection.

A capture of the TerraTerm terminal is attached which displays the successful transmission of the buffer that we initialized in the main.c file.  
