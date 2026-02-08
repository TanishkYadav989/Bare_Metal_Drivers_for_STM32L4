In this External Interrupt Driver, I have used GPIO Port B pin 9 for a button input.
So for the Button, I have confirgured the GPIO pin for Input Mode (01) with a pull up setting for the input by PUPD register.
Because I'm using pin 9 for input so for that i need to configure EXTI_9 as the same pin numbers are multiplexed to their same EXTI number.
For Setting that EXTI, we first set the SYSCFG register 3 bit 4-7.
To detect when the button is released, We disabled the Rising Trigger & enabled the Falling Trigger pin 9.
Now enabling pin 9 of Interrupt Mask register to make the interrupt visible to the Processor.
Finally we allot the priority of the Interrupt as 0x10 (8 bit as only upper four bits determine it) & enabling it by ISER0 beacuse it is at 23 position in the Vector Table.

EXTI_9_5_IRQHandler handles the function that the Interrupt will perform.
Inside this we look for the Pending Interrupt Flag in Pending Register 1 & Clears the Flag by setting PR1 pin 9 when its role is finished.
