In our smartphones or cars there is a volume system that manipulates the volume intensity when a button or a knob is pressed/rotated. The further we rotate the knob or longer we press the button the volume raises and if the volume reaches 75% a warning message pops up with a user input that overrides the capped intensity & allows to raise volume even further.

This system combines two real-world behaviors - the position control of car infotainment volume knob & hearing protection warning in smartphones using Timer-Triggered ADC, Data transfer with DMA & PWM duty cycle manipulation for the LED. When the intensity reaches a threshold of 75%, it halts the further rise in the LED's intensity until the button is pressed to override the warning & continue its operations.

GPIO pins Used :
1. PA0 used for the Analog Input from a 10kohm Potentiometer powered by the 3.3V pin.
2. PB0 used for the PWM powered LED.
3. PC0 used for the buzzer output powered by the 5V pin.
4. PB9 used for the button input for warning override.

 ADC Configuration :
 1. First we enable analog input for PA0 as it is mapped to ADC1.
 2. Configuring Timer 4 to trigger the analog conversion from the potentiometer by enabling update event with TRGO.
 3. Then we select the clock source for ADC1 from CCIPR & set it to use the system clock i.e 4MHz along with enabling its clock on AHB Bus-2.
 4. Clearing the ADC Common register to set the prescaler/clock divider to 0 & as asynchronous clock mode.
 5.  Configuring CR we enable the Analog voltage regulator with a delay to enable it properly. Then start the ADC calibration and wait until it is done.
 6.  Now we configure CFGR, enabled DMA & its circular mode for continuous reading, enabling & mapping Analog Watchdog 1 to CH_5 for 75% threshold detection. Mapping TIM4 for TRGO for CH_5. Setting hardware trigger on rising edge.
 7.  The upper (3068) & lower (0) threshold is assigned in TR1 which is linked to AWD1.
 8.  Setting the Sampling rate to 247.5 ADC clock cycles & allotting ADC1 CH_5 as the 1st sequence. AWD1's interrupt is also enabled to check its flag in ADC1's ISR.
 9.  The DMA1 CH_1 is also configured in the same function by setting PSIZE & MSIZE to 8-bits, enabling circular mode, TCIE & TEIE. The address of the global buffer, the ADC1 regular channel data register & the size of the buffer are assigned to CMAR, CPAR & CNDTR respectively. CSELR's Ch1 is cleared as DMA1's mapping for ADC1 is represented by 0000. Lastly the DMA interrupt & the data stream is enabled before enabling ADC1.
 10.   ADC1 is enable along with its interrupt & we wait until the ADC ready flag is set in the status register. When its ready we intitate the conversion by setting ADC start bit in CR.
 11.   
