This driver is used for sending text to PC using the in-built UART (Serial Communication Protocol) via the ST-LINK & USB port of my nucleo-board, because of this i didn't have to use an external USB-TTL. This driver is generic and can be used for any UART protocol.
For this, I have configured GPIO PA2 & PA3 as Alternate function for TX & RX respectively of USART_2.
For the USART initialisation, I have firstly disabled USART_2 for alloting its functions & for its proper functioning as several bits can't be set without disabling it.
I have cleared bit 28 & 12 which are the M0 & M1 bits respectively used for defining the word length. Here (00) for 8 data bits & 1 start bit.
Then i have set bit 15 which is OVER8 bit defining the oversampling rate. Here (1) for oversampling by 8.
Now in the Control Register 2, I have cleared bits 12 & 13 for determining STOP bits. Here, (00) for 1 STOP bit.
After clearing bit 10 for disabling polarity control, I have set the Baud Rate register which is determined by the function argument 'brr'.
Finally I have enabled Transmitter & Reciever & the USART_2 protocol itself.
In polling method, the CPU only look after a particular bit/flag to be flipped. Here it looks for the TEACK(Transmitter Enable Acknowledge Flag) & REACK (Reciever Enable Acknowledge Flag) in the Status Register.

Further i have declared two more functions;
1. USART_Write-> This enables the transmitting of data from the Controller to the PC. In this function the CPU looks for the TXE flag & assign TD register with the buffer array & setting the TC flag.
2. USART_Read -> This enables recieving data from an external device by the Controller. In this the CPU looks for the RXNE Flag & assign the data recieved to the Buffer array.

The Formula used for Baud Rate Calculation;
    USARTDIV (hex) = ((1+OVER8) * Processor Clock) / Baud
   { If OVER8 = 1 (Oversampling by 8, 0 for 16),
          the fractional part of the number i.e the last digits is shifted towards right by 1 & then along the rest.
    }      
  
                
