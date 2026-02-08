In this LED Driver, I have configured the GPIO Port C pin 0.
Configured the Mode Register by clearing and setting bit-1 of Pin 0 for General Purpose Output Mode (00).
Setting Output type as Push-Pull.
Clearing the Pull up Pull down register for No pull up/No pull down (00).
Clearing the Output Speed register for Low Speed Output (00).

Further if have created two fuctions:
1. ENABLE_LED() - Setting the Output Data register of PC0 for Lighting the LED connected to that port.
2. TOGGLE_LED() - Toggling the bit 0 of ODR for Blinking the Led.
