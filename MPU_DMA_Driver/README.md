This driver is Integration of I2C MPU6050 Sensor reading with DMA (Direct Memory Access) for Automated High-Speed Data transfer between the Slave & Master.
I2C is a Synchronous Half-Duplex protocol i.e transmission & reception cannot take place simultaneously.
All the Slave devices connected to the I2C bus are accessed through their own unique 7-bit or 10-bit address.
The total no. of Slaves connected to an I2C bus is limited by the Bus Capacitance (Sum of the Parasitic Capacitance) i.e 400pF.

For combining I2C & DMA, a proper conversation initiating sequence for both TX & RX is provided in the Reference Manual (RM0351) i.e DMA for transfer/reception should be fully initialized & armed before the START bit is set & the end of transfer is determined by the NBYTES counter in I2C CR2.

The I2C configuration is same as the previous one, only the TXEIE & ERRIE are enabled for I2C Bus & DMA transfer hang.
Both the I2C Event Handler & Error Handler are enabled via NVIC.
For the DMA Channel 6 & 7 (each for TX & RX) initialization, Peripheral Size & Memory Size are set for 8-Bit data & Memory Increment mode is used.
TCIE, HTIE & TEIE are enabled for tracking the status of the DMA Transfer.

Two Static Functions are initialized for Initiating the Transmission & Reception of Data via I2C,
1. I2C_Start -> This contains the allocating of Memory Address (the source), Peripheral DR Address (the destination) & the Size of the Data to DMA Channel 6 & enabling DMA & TXDMAEN for I2C at last for data request. Now the DMA is armed, we can start the conversation by Configuring CR2 of I2C with the Slave Address & Data size along with AUTOEND, write condition & finally the START bit to initiate.

      I2C_transmit checks for the data ready flag & links to the static Start function. If it is ready, the conversation is started. 

2. I2C_read -> This is same as the I2C_start function, DMA Channel 7'S CMAR (RXDR), CPAR (RxBuffer) & CNDTR (size) are assigned. In this the RXDMAEN is set to receive data via DMA stream. The CR2 is set with Slave Address, data size, AUTOEND, START bit & a Read Condition to initiate reception.

    I2C_receive checks for the data ready flag & links to the static Read function. If it is ready, the      conversation is started.
 In the I2C Error, DMA Channel 6 & 7 ISRs, we clear the flags and set LEDs connected via GPIO pins for visual feedback of the errors in Sensor conversations & Data transfer streams.

In the I2C Event Handler, we check for the STOPF flag to detect if the ongoing Transmission is over.
In the if statement for the STOPF, we use a State Machine approach to talk to the sensor in a fixed sequence using the Globally initialized State_t Enum.
The First two steps are for setting up the MPU for the Data reading by incrementing the state. Then in the third step we initialize the Read sequence for the Data register and increments the state.
In the Last State we again send the Data Register Address & loops the State back to the previous read one & sets the Data Ready flag.
This State Machine Approach helps us to read the Sensor burst easily without creating CPU Overhead & use of general loops. The CPU is free during this whole process.

Formula's Used:

1. SCLL -> (SCLL+1)*(Tpsc)
2. SCLH -> (SCLH+1)*(Tpsc)
3. SCLDEL -> (SCLDEL+1)*(Tpsc)
4. SDADEL -> (SDADEL+1)*(Tpsc)
   
  Sum of SCLH & SCLL should be around 100 as the transfer speed of I2C is 100 KHz.
 
*These values should be kept above the minimum requirement provided for each of them in the Slave's Datasheet.
Atomic Register configuration was used for CR2 of I2C to prevent race conditions as it was a major bug that resulted in the I2C bus hang.*

Logic Analyzer & Terminal reading capture is attached for the same.
