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

#include "driverlib/adc.h"

#include "driverlib/uart.h"

#include "driverlib/interrupt.h"

#include "utils/uartstdio.h"


void
InitConsole(void)
{

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);


    /* Make the UART pins be peripheral controlled. */
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //
    // Initialize the UART for console I/O.
    //
    UARTStdioInit(0);
}

int main(void)
{
	unsigned long temp[8];
	/*Set the clocking to directly run from the crystal at 8MHz*/
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);



	/* Set the clock for the GPIO Port C */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	/*Enable ADC Peripheral*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

	/* Set the type of the GPIO Pin */
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_4);
	
    /* GPIO Pins 5, 6, 7 on PORT C initialized to 0 */
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_4, 0);
	SysCtlDelay(SysCtlClockGet()/10);

	/* UART config */
	InitConsole();

	/*Configure ADC Peripheral*/
	/*Configure ADC Sequence*/
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	/*Enable ADC sequence*/
	ADCSequenceDisable(ADC0_BASE, 0);
	SysCtlDelay(SysCtlClockGet()/10);
	ADCSequenceEnable(ADC0_BASE, 0);


   /*Clear ADC Interrupt*/
	ADCIntClear(ADC0_BASE, 0);

	IntMasterEnable();

	while(1)
	{

    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH5 | ADC_CTL_IE | ADC_CTL_END);

    ADCProcessorTrigger(ADC0_BASE, 0);
    while(!ADCIntStatus(ADC0_BASE, 0, false))
    {
    }
    ADCIntClear(ADC0_BASE, 0);

    ADCSequenceDataGet(ADC0_BASE, 0, temp);

    UARTprintf("X%04d\n", temp[0]);
	SysCtlDelay(SysCtlClockGet()/5);


	}

}
