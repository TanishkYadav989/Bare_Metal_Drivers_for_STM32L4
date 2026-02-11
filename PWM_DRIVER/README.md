I have written this driver for a Breathing LED effect and a working Siren using a passive buzzer.
For this i have used Timer_3 which is a general purpose timer & GPIO Port B pin 0 which has an Alternate Function with that Timer.

Firstly, I have configured PB0 with an Input Mode along with Alternate Function in the AFRL register pin 0.
Then I have intialised Timer_3 by activating its clock and setting it Centre-Aligned mode.
After alotting values for the Prescaler register, Auto-Reload Register & the Capture/Compare Register(Counter/Duty Cycle), I have bit 2 & 3 for PWM Mode 1.
Lastly, I have enabled Capture/Compare mode 3 & the Timer itself.

For Calculating the Prescaler, I have used the formula;
         PSC = (Clock Frequency of Processor /  Clock Counter) - 1

 For Calculating the Duty Cycle;
          Duty Cycle = CCR/ARR to 1-(CCR/ARR)
