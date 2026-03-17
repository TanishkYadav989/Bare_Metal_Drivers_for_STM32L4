This driver is the implementation of the I2C (Inter-Integrated Circuit) communication protocol to read the MPU-6050 sensor which is a combined sensor module of Accelerometer, Gyro & Temperature sensors.
I2C is a Synchronous Half-Duplex protocol i.e transmission & reception cannot take place simultaneously. Two lines are used, SCL (for Clock Generation) & SDA (for Data Transfer/Reception).
Since I2C uses an Open-Drain output type, external pull-up resistors are needed for Proper Transmission.
All the Slave devices connected to the I2C bus are accessed through their own unique 7-bit or 10-bit address.
The total no. of Slaves connected to an I2C bus is limited by a the Total Bus Capacitance.

PB8 & PB9 of GPIOs is mapped as the SCL & SDA Alternate Function pins for I2C1.
For the Configuration of the I2C communication, we first have to Enable all the interrupts for I2C i.e RXEIE TXEIE, NACKIE, STOPIE, TCIE & ERRIE.
The TIMING Registers needs to configured according to the Minimum requirements (PSC, SCLL, SCLH, SDADEL, SCLDEL) of the Slave Device (MPU6050 here) that we are using.
Next we allot are Controller with an Address of its owm address via the OAR1 register.
Now NACK bit is enabled to receive a Not-Acknowledge after a byte is send.
Finally the I2C1 is enabled along with its Interrupt through NVIC.

For initialising the Start Bit to start the conversation between the Master & the Slave, an atomic write of the CR2 is used to prevent Race Conditions.
In this function we allot the Slave Address, the size of the data to be transmitted. Autoend is also enabled to send STOP Bit automatically when the communication is over.
Finally a Start Bit is set to initiate the conversation between the master & slave.
