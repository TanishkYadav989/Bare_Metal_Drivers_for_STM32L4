This driver is for the System Timer a.k.a SysTick which is a System Interrupt.
We have Intialised the systick function with an argument Ticks which is the Frequency to be set for the timer.
Now load is defined which is a counter with Ticks-1 as the timer starts from 0 in the Systick Reload value register.
Value is set as 0 which is defined as the start position in the SysTick VAL register.
Further we set the priotity of the interrupt using SHPR which is set as lowest priority, and 11 is the Interrupt number of SysTick in Vector Table.
And lastly, Enabling the System Timer by setting its first three bits i.e Clock Source, TICKINT & Enable bits.

In the Interrupt Handler, we decrement the value of t which is defined globally until decrements to 0.
And the delay function sets the precise time for the counter to generate a delay when t is not equal to 0.
