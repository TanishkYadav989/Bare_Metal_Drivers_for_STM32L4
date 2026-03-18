This driver is Intergration of I2C MPU6050 Sensor reading with DMA (Direct Memory Access) for Automated High-Speed Data transfer between the Slave & Master.
I2C is a Synchronous Half-Duplex protocol i.e transmission & reception cannot take place simultaneously.
All the Slave devices connected to the I2C bus are accessed through their own unique 7-bit or 10-bit address.
The total no. of Slaves connected to an I2C bus is limited by a the Bus Capacitance (Sum of the Parasitic Capacitance) i.e 400pF.

For combining I2C & DMA, a proper conversation initiating sequence for both TX & RX is provided in the Reference Manual (RM3051) i.e DMA for transfer/reception should be fully initialized & armed before the START bit is set & the end of transfer is determined by the NBYTES counter in I2C CR2.

The I2C configuration is same as the previous one, only the TXEIE & ERRIE are enabled for I2C Bus & DMA transfer hang.
Both the I2C Event Handler & Error Handler are enabled via NVIC.
For the DMA Channel 6 & 7 (each for TX & RX) initialisation, Peripheral Size & Memory Size are set for 8-Bit data & Memory Increment mode is used.
TCIE, HTIE & TEIE are enabled for tracking the status of the DMA Transfer.

Two Static Functions are initialised for Initiating the Trnasmission & Reception of Data via I2C,
1. I2C_Start -> This contains the Allotting of Memory Addrress (the source), Peripheral DR Address (the destination) & the Size of the Data to DMA Channel 6 & enabling DMA & TXDMAEN for I2C at last for data request. Now the DMA is armed, we can start the conversation by Configuring CR2 of I2C with the Slave Address & Data size along with AUTOEND, Write condition & finally the START bit to initiate.

I2C_transmit checks for the data ready flag & links to the static Start function. If it is ready, the conversation is started. 

2. I2C_read ->
