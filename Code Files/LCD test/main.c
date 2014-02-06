 /* Centre for Electronic Design and Technology, NSIT, New Delhi
 * 
 * Dhananjay V Gadre, dhananjay.gadre@gmail.com
 * Shanjit Singh Jajmann, shanjitsingh@gmail.com
 * Rohit Dureja, rohit.dureja@gmail.com
 * 
 */

/* Defines the base address of the memories and peripherals */
#include "inc/hw_memmap.h"

/* Defines the common types and macros */
#include "inc/hw_types.h"

/* Defines and Macros for GPIO API */
#include "driverlib/gpio.h"

/* Prototypes for the system control driver */
#include "driverlib/sysctl.h"

/* Utility driver to provide simple UART console functions. */
#include "utils/uartstdio.h"


int main(void)
{

	/* Set the clocking to directly run from the crystal at 8MHz */
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

	/* Set the clock for the GPIO Port C */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	//SysCtlPeripheralDisable(SYSCTL_PERIPH_GPIOC);

	/* Set the type of the GPIO Pins */
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	
	/* GPIO Pins 5, 6, 7 on PORT C initialized to 1
	 * All LED's Off.
	 *  */
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	
	GPIOPinWrite(GPIO_PORTC_BASE, 0x00000020 , 0x00000020);
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6 , 0);

	GPIOPinWrite(GPIO_PORTC_BASE, 0x000000FF , 0x000000FF);


	unsigned long count;
	
	/* Infinite loop */
	while(1)
    {
		
		/* Make Pin Low */
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0);

        /* Delay for a while */
        for(count=0;count<200000;count++)
        {
		}
		
		/* Make Pin High */
		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_PIN_5); 
		
		/* Delay for a while */
		for(count=0;count<200000;count++)
		{
		}



    }


}








