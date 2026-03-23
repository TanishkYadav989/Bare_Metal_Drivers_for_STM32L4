This driver is the implementation of the I2C (Inter-Integrated Circuit) communication protocol to read the MPU-6050 sensor, which is a combined sensor module of Accelerometer, Gyro & Temperature sensors.
I2C is a Synchronous Half-Duplex protocol i.e transmission & reception cannot take place simultaneously. Two lines are used, SCL (for Clock Generation) & SDA (for Data Transfer/Reception).
Since I2C uses an Open-Drain output type, external pull-up resistors are needed for Proper Transmission.
All the Slave devices connected to the I2C bus are accessed through their own unique 7-bit or 10-bit address.
The total no. of Slaves connected to an I2C bus is limited by the Bus Capacitance (Sum of the Parasitic Capacitance) i.e 400pF.

PB8 & PB9 of GPIOs is mapped as the SCL & SDA Alternate Function pins for I2C1.
For the Configuration of the I2C communication, we first have to Enable all the interrupts for I2C i.e RXEIE TXEIE, NACKIE, STOPIE, TCIE & ERRIE.
The TIMING Registers needs to configured according to the Minimum requirements (PSC, SCLL, SCLH, SDADEL, SCLDEL) of the Slave Device (MPU6050 here) that we are using.
Next, we allot are Controller with an Address of its own address via the OAR1 register.
Now NACK bit is enabled to receive a Not-Acknowledge after a byte is send.
Finally, the I2C1 is enabled along with its Interrupt through NVIC.

For initializing the Start Bit to start the conversation between the Master & the Slave, an atomic write of the CR2 is used to prevent Race Conditions.
In this function we allot the Slave Address, the size of the data to be transmitted. AUTOEND is also enabled to send STOP Bit automatically when the communication is over.
Finally, a Start Bit is set to initiate the conversation between the master & slave.

Then, we initialize a Send & a Receive Function to map the size & data arguments to the Globally initialized Buffers & Variables & call the Start Functions inside them.
In the ISR for the I2C1 we check for the RXNE & TXE flag. If they are set, data is manually transferred by reading & writing to/from the global buffers. If the STOP flag is set the buffers are emptied for new data. If the NACK flag is set, a STOP bit is end manually to end conversation.

Two other functions exclusively for the MPU are initialized for Reading & Writing to the MPU,
1. MPU_Send -> We initialize an array containing the Register Address & its Value. Then this array is sent to the MPU using the I2C_send function.
2. MPU_Receive -> We send Read Register Address to access the MPU's Data Register with a Read Condition & Start a Data Receive sequence to the Buffer allotted to the Function Argument.

Formula's Used:
1. SCLL -> (SCLL+1)*(Tpsc)
2. SCLH -> (SCLH+1)*(Tpsc)
3. SCLDEL -> (SCLDEL+1)*(Tpsc)
4. SDADEL -> (SDADEL+1)*(Tpsc)
5. Sum of SCLH & SCLL should be around 100 as the transfer speed of I2C is 100 KHz.
   
These values should be kept above the minimum requirement provided for each of them in the Slave's Datasheet.

Logic Analyzer Capture is attached for the same.
