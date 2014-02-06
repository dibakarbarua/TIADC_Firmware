/* Centre for Electronic Design and Technology, NSIT, New Delhi
   Dibakar Barua */

/* Defines the base address of the memories and peripherals */
#include "inc/hw_memmap.h"

/* Defines the common types and macros */
#include "inc/hw_types.h"

/* Defines and Macros for GPIO API */
#include "driverlib/gpio.h"

/* Prototypes for the system control driver */
#include "driverlib/sysctl.h"

/* Header files for various peripherals */

#include "driverlib/adc.h"

#include "driverlib/uart.h"

#include "driverlib/interrupt.h"

#include "utils/uartstdio.h"

/* Prototypes for the SysTick driver. */
#include "driverlib/systick.h"

/*macros for GPIO digital bases*/

#define digital2base GPIO_PORTB_BASE
#define digital3base GPIO_PORTB_BASE
#define digital4base GPIO_PORTB_BASE
#define digital5base GPIO_PORTB_BASE
#define digital6base GPIO_PORTB_BASE
#define digital7base GPIO_PORTB_BASE
#define digital8base GPIO_PORTC_BASE
#define digital9base GPIO_PORTD_BASE
#define digital10base GPIO_PORTA_BASE
#define digital11base GPIO_PORTA_BASE
#define digital12base GPIO_PORTA_BASE
#define digital13base GPIO_PORTA_BASE

/*macros for GPIO digital pins*/

#define digital2pin GPIO_PIN_5
#define digital3pin GPIO_PIN_4
#define digital4pin GPIO_PIN_1
#define digital5pin GPIO_PIN_0
#define digital6pin GPIO_PIN_6
#define digital7pin GPIO_PIN_7
#define digital8pin GPIO_PIN_4
#define digital9pin GPIO_PIN_0
#define digital10pin GPIO_PIN_3
#define digital11pin GPIO_PIN_5
#define digital12pin GPIO_PIN_4
#define digital13pin GPIO_PIN_2

/*macros for digital relay control*/
#define relaycontrolbase digital8base
#define relaycontrolpin	digital8pin

/*macros for digital speaker control*/
#define speakerbase digital9base
#define speakerpin digital9pin

/*LCD Pins connection
LCD			Guru
D4			D2(PB5)
D5			D3(PB4)
D6			D4(PB1)
D7			D5(PB0)
E			D6(PB6)
RS			D7(PB7)
R/W			GND
*/

/*Variables*/

//Store Sensor Values
unsigned long temp[8];

//for checking eye blinks/eyes being closed
unsigned int blinkcounter = 0;

//loop variables
unsigned long i = 0;
unsigned long count = 0;
unsigned long counter = 0;

//Variables to Store sensor Values
unsigned long eyesense = 0;
unsigned long alcoholsense = 0;
unsigned long butval = 0;
unsigned long xval = 0;
unsigned long zval = 0;
unsigned int deltax = 0;
unsigned int deltaz = 0;

// macros for LCD Commands
#define CLEARSCREEN 0x1
#define GOTO_FIRSTROW_THIRDCOLUMN 0x82
#define GOTO_FIRSTROW_FIFTHCOLUMN 0x84
#define GOTO_SECONDROW_THIRDCOLUMN 0xC3
#define GOTO_SECONDROW_FIRSTCOLUMN 0xC0

/* Delay Function */
void delay(unsigned long ulSeconds)
{
    /* Loop while there are more seconds to wait. */
	while(ulSeconds--)
    {
        /* Wait until the SysTick value is less than 1000. */
        while(SysTickValueGet() > 1000)
        {
        }

        /* Wait until the SysTick value is greater than 1000. */
        while(SysTickValueGet() < 1000)
        {
        }
    }
}

/* Function definitions for the LCD */

void LCD_Command(char ch)
{
	int y=2;
	while(y>0)
	{
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_6);
	    if(y==2)
	    {
	      GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0| GPIO_PIN_1| GPIO_PIN_4| GPIO_PIN_5, 0x00000000|((ch&(1<<7))>>7)|((ch&(1<<6))>>5)|((ch&(1<<5))>>1)|((ch&(1<<4))<<1));
	    }
	    if(y==1)
	    {
	    	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0| GPIO_PIN_1| GPIO_PIN_4| GPIO_PIN_5, 0x00000000|((ch&(1<<3))>>3)|((ch&(1<<2))>>1)|((ch&(1<<1))<<3)|((ch&(1<<0))<<5));
	    }
	    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6, 0);
	    delay(2);
	    y--;
	}
}

void PrintStringOnLCD(char * ch)
{
	int i=0;
	for(i=0; ch[i]!='\0';++i)
	{
		int y=2;
		while(y>0)
		{
			GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_6|GPIO_PIN_7);
			if(y==2)
			{
			  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4|GPIO_PIN_5, 0x00000000|((ch[i]&(1<<7))>>7)|((ch[i]&(1<<6))>>5)|((ch[i]&(1<<5))>>1)|((ch[i]&(1<<4))<<1));
			}
			if(y==1)
			{
				GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4|GPIO_PIN_5, 0x00000000|((ch[i]&(1<<3))>>3)|((ch[i]&(1<<2))>>1)|((ch[i]&(1<<1))<<3)|((ch[i]&(1<<0))<<5));
			}
			GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6, 0);
			delay(2);
			y--;
		}
	}
}

void SetFourBitMode(char ch)
{
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_6);
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0| GPIO_PIN_1| GPIO_PIN_4| GPIO_PIN_5, 0x00000000|((ch&(1<<7))>>7)|((ch&(1<<6))>>5)|((ch&(1<<5))>>1)|((ch&(1<<4))<<1));
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6, 0);
	delay(1);
}

void ConfigureLCD()
{
		SetFourBitMode(0x30);	//Eight bit mode

	    SetFourBitMode(0x30);	//Second attempt to get into eight bit mode

	    SetFourBitMode(0x20);	//Set LCD in 4 bit mode

	    LCD_Command(0x2);  //Return home

	    LCD_Command(CLEARSCREEN);  //Clear display

	    LCD_Command(0x6);  //Cursor move right

	    LCD_Command(0xC);  //Display control: Display on, cursor off and cursor blink off

}

void PrintIntonLCD(unsigned int var)
{
	char str[5];
	short i=0;
	int temp=10;
	while(1)	//Temp should ultimately store 10^(No. of digits in var-1)
	{
		if(var/temp==0)
			break;
		else
			temp*=10;
	}
	temp/=10;

	while(temp>0)
	{
		str[i]=var/temp+48;
		var%=temp;
		temp/=10;
		i++;
	}
	str[i]='\0';
	PrintStringOnLCD(str);
}

void InitConsole(void)
{
    /* Make the UART pins be peripheral controlled. */
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	//Initialize UART
	UARTConfigSetExpClk(UART0_BASE, 8000000, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	UARTEnable(UART0_BASE);

	// Initialize the UART for console I/O.
     UARTStdioInit(0);

}

/*making the speaker play an alarm bell with an infinitesimally small delay*/

void speakerplay(void)
{
	for(count=0; count <= 1000; count++)
			{
			/* Make Pin Low */
	        GPIOPinWrite(speakerbase, speakerpin, speakerpin);

	        SysCtlDelay(SysCtlClockGet()/5000);

			/* Make Pin High */
	        GPIOPinWrite(speakerbase, speakerpin, 0);

			/* Delay for a while */
			 SysCtlDelay(SysCtlClockGet()/5000);

			}

			SysCtlDelay(SysCtlClockGet()/50);
			GPIOPinWrite(speakerbase, speakerpin, 0);

			SysCtlDelay(SysCtlClockGet()/50);

}

/*trigger this alert condition if alcohol is detected. Driver cannot revert back to a previous state. System
shuts down. Message is Sent and alarm will constantly ring.
 */

void alcoholdetected(void)
{
       /*Send message using GPS*/

       //ENTER FUNCTION FOR GSM MESSAGE HERE

       LCD_Command(CLEARSCREEN);
       LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
       LCD_PrintStringOnLCD("Alcohol");
       LCD_Command(GOTO_SECONDROW_THIRDCOLUMN);
       LCD_PrintStringOnLCD("DetectedSTOP");
       UARTprintf("Alcohol Detected. \n\nSTALL SYSTEM \n\n");
       SysCtlDelay(SysCtlClockGet()/3);

       while(1)
       {

      	   GPIOPadConfigSet(relaycontrolbase, relaycontrolpin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
      	   GPIOPinWrite(relaycontrolbase, relaycontrolpin, relaycontrolpin); //To signal help

           for(i=0; i<=100; i++)
           {
         	  speakerplay();
           }

           GPIOPadConfigSet(relaycontrolbase, relaycontrolpin, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
           GPIOPinWrite(relaycontrolbase, relaycontrolpin, 0);


       }
}


void alarmState()
{
           LCD_Command(CLEARSCREEN);
           LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
           LCD_PrintStringOnLCD("ALARM");
           LCD_Command(GOTO_SECONDROW_THIRDCOLUMN);
           LCD_PrintStringOnLCD("TRIGGERED");
           UARTprintf("Alarm Triggered \n\n");

           SysCtlDelay(SysCtlClockGet()/5);

           /* run a  loop 800000 times to give driver time to stop alarm */

           for(i=0; i<=800000; i++)
           {

                /* For reading the button */
                /* Just this code fragment has to be repeated to read the ADC again */
                /* Sample Sequence Config. done just once in main() */

                ADCProcessorTrigger(ADC0_BASE, 0);
   	            while(!ADCIntStatus(ADC0_BASE, 0, false))
   	            {
                }

   	            ADCSequenceDataGet(ADC0_BASE, 0, temp);

   	            /*Check if button was pressed*/

     	        if(temp[0] < 1000)
		        {

			    /*Debounce the button if it IS READ */

                    for(count=0; count<=2000; i++)
                    {
                    }
                    if(temp[0] < 1000)
                    {
                        /*if driver presses the button escape the loop */
                        LCD_Command(CLEARSCREEN);
                        LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
                        LCD_PrintStringOnLCD("BUTTON");
                        LCD_Command(GOTO_SECONDROW_THIRDCOLUMN);
                        LCD_PrintStringOnLCD("PRESSED");
                        UARTprintf("Button Pressed \n\n");
                        UARTprintf("Button Val = %04d\n", temp[0]);


                        SysCtlDelay(SysCtlClockGet()/5);
                        break;
                    }
                }

                /*If button is not pressed then initiate alarm sequence */

                LCD_Command(CLEARSCREEN);
                LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
                LCD_PrintStringOnLCD("BUTTON NOT");
                LCD_Command(GOTO_SECONDROW_THIRDCOLUMN);
                LCD_PrintStringOnLCD("PRESSED");
                UARTprintf("Button Not Pressed \n\n");
                SysCtlDelay(SysCtlClockGet()/5);
                UARTprintf("Button Val = %04d\n", temp[0]);

                SysCtlDelay(SysCtlClockGet()/5);

                /*Sound the alarm everytime the loop is entered */

                GPIOPadConfigSet(relaycontrolbase, relaycontrolpin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); //Sink Current is 2mA
                GPIOPinWrite(relaycontrolbase, relaycontrolpin, relaycontrolpin);

                for(count=0; count<=5; count++)
                {
                    speakerplay();
                }

                GPIOPadConfigSet(relaycontrolbase, relaycontrolpin, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
                GPIOPinWrite(relaycontrolbase, relaycontrolpin, 0);

                SysCtlDelay(SysCtlClockGet()/3);

           }

           /* If the stipulated time elapses and the driver hasnt pressed the button */
           /* Enter and infinite loop and keep sounding the alarm */
           /* SEND a GSM Text Message */

           if(i == 800000)
           {

                LCD_Command(CLEARSCREEN);
                LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
                LCD_PrintStringOnLCD("NO REACTION");
                LCD_Command(GOTO_SECONDROW_THIRDCOLUMN);
                LCD_PrintStringOnLCD("SYSTEM STALL");
                UARTprintf("No Reaction from Driver System Stalled \n");

                SysCtlDelay(SysCtlClockGet()/5);

                /*Send message using GPS*/

                //ENTER FUNCTION FOR GSM MESSAGE HERE


                /*System will stall NOW */
                while(1)
                {

                    GPIOPadConfigSet(relaycontrolbase, relaycontrolpin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
                    GPIOPinWrite(relaycontrolbase, relaycontrolpin, relaycontrolpin);

                    for(count=0; count<=100; i++)
                    {
                        speakerplay();
                    }

                    GPIOPadConfigSet(relaycontrolbase, relaycontrolpin, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
                    GPIOPinWrite(relaycontrolbase, relaycontrolpin, 0);
                }
           }

           /* If the driver presses the button within the stipulated time */
           /* Escape the function */

           else
           {

                LCD_Command(CLEARSCREEN);
                LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
                LCD_PrintStringOnLCD("ALARM");
                LCD_Command(GOTO_SECONDROW_THIRDCOLUMN);
                LCD_PrintStringOnLCD("ESCAPED");
                UARTprintf("Alarm State Escaped \n");

                /*Turn Display Off if alarm is escaped */

                GPIOPadConfigSet(relaycontrolbase, relaycontrolpin, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
                GPIOPinWrite(relaycontrolbase, relaycontrolpin, 0);

                SysCtlDelay(SysCtlClockGet()/2);

           }

}

int main(void)

{
            /*Set the clocking to directly run from the crystal at 8MHz*/
            SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

            /* Set the clock for the GPIO Ports */
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

            /*Enable ADC Peripheral*/
            SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

            /* Set the type of the GPIO Pins as DIGITAL OUTPUTS */

            GPIOPinTypeGPIOOutput(digital2base, digital2pin);
            GPIOPinTypeGPIOOutput(digital3base, digital3pin);
            GPIOPinTypeGPIOOutput(digital4base, digital4pin);
            GPIOPinTypeGPIOOutput(digital5base, digital5pin);
            GPIOPinTypeGPIOOutput(digital6base, digital6pin);
            GPIOPinTypeGPIOOutput(digital7base, digital7pin);
            GPIOPinTypeGPIOOutput(digital8base, digital8pin);
            GPIOPinTypeGPIOOutput(digital9base, digital9pin);
            GPIOPinTypeGPIOOutput(digital10base, digital10pin);
            GPIOPinTypeGPIOOutput(digital11base, digital11pin);
            GPIOPinTypeGPIOOutput(digital12base, digital12pin);

            /*Initialize digital control pins*/

            GPIOPadConfigSet(relaycontrolbase, relaycontrolpin, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU); // 8 mA sink current required
            GPIOPinWrite(relaycontrolbase, relaycontrolpin, 0); //relay switch off inside main() function

            GPIOPinWrite(speakerbase, speakerpin, 0); //speaker off inside main() function

            /* UART config */
            InitConsole();

            /* Enable is set low */
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6, 0);

            /* Set up the period for the SysTick timer.  The SysTick timer period will
            be equal to 1ms.*/
            SysTickPeriodSet(SysCtlClockGet()/1000);

            /* Enable SysTick. */
            SysTickEnable();

            ConfigureLCD();

            /*WELCOME SCREEN */

            LCD_Command(GOTO_FIRSTROW_FIFTHCOLUMN);
            PrintStringOnLCD("WELCOME");
            UARTprintf("Welcome \n");
            delay(2000);

            /*Configure the ADC Sequence for the FIRST and ONLY TIME*/
	    	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

	    	/*Enable ADC sequence*/
	    	ADCSequenceDisable(ADC0_BASE, 0);
	   	    ADCSequenceEnable(ADC0_BASE, 0);
	   	    delay(10);
	   	    ADCIntClear(ADC0_BASE, 0);

	   	    /*Clear ADC Interrupt*/
	        ADCIntClear(ADC0_BASE, 0);

	        /*Configure ADC Sample Collection from 4 Channels READ ALL OTHER SENSORS*/

            ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0); //button
            ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1); //z axis
            ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH3); // x axis
            ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH4); // IR sensor
            ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH5 | ADC_CTL_IE | ADC_CTL_END); //alcohol

            /*

            According to above Sequence of steps
            temp[o] ---> button
            temp[1] ---> z axis
            temp[2] ---> x axis
            temp[3] ---> IR Sensor
            temp[4] ---> alcohol Sensor

            */

            /* START calculating the MEAN VALUES for acceleromter (AUTO CALIBRATION) */

            unsigned long sumx = 0;
            unsigned long sumz = 0;
            unsigned int xref = 0;
            unsigned int zref = 0;

            SysCtlDelay(SysCtlClockGet()/5);

            for(i=0; i<= 100; i++)
            {

                SysCtlDelay(SysCtlClockGet()/50);

                /*the sequence given below is used to READ the sensors
                at any point from now on */
                /* Just use the appropriate temp[i] values for analysis */

                /* SEQUENCE STARTS */

                ADCProcessorTrigger(ADC0_BASE, 0);
                while(!ADCIntStatus(ADC0_BASE, 0, false))
                {
                }
                ADCSequenceDataGet(ADC0_BASE, 0, temp);

                /* SEQUENCE ENDS */

                sumx = sumx + temp[2];
                sumz = sumz + temp[1];
                SysCtlDelay(SysCtlClockGet()/50);

            }

            xref = sumx/101;
            zref = sumz/101;

            LCD_Command(CLEARSCREEN);
            LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
            LCD_PrintStringOnLCD("Xref = ");
            LCD_PrintIntOnLCD(xref);
            UARTprintf("Xref = %04d\n", xref);
            delay(3000);

            LCD_Command(CLEARSCREEN);
            LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
            LCD_PrintStringOnLCD("Yref = ");
            LCD_PrintIntOnLCD(yref);
            UARTprintf("Yref = %04d\n", yref);
            delay(3000);

            LCD_Command(CLEARSCREEN);

            /*end of auto calibration */
            while(1)
            {
                /*Reinitiate blinkcounter for eyes CLOSED*/
                blinkcounter = 0;

                /*Reading just IR sensor values in a loop */

                for(i=0; i<= 15; i++)
                {
                    ADCProcessorTrigger(ADC0_BASE, 0);

                    while(!ADCIntStatus(ADC0_BASE, 0, false))
                    {
                    }

                    ADCSequenceDataGet(ADC0_BASE, 0, temp);

                    if(temp[3] < 1000)
                    {
                        blinkcounter++;
                    }

                    UARTprintf("IR value %04d\n", temp[3]);
                    LCD_Command(CLEARSCREEN);
                    LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
                    LCD_PrintStringOnLCD("IRval =");
                    LCD_PrintIntOnLCD(temp[3]);
                    delay(1000);

                }

                /* Read ALL other sensors ONCE */

                ADCProcessorTrigger(ADC0_BASE, 0);

                while(!ADCIntStatus(ADC0_BASE, 0, false))
                {
                }

                ADCSequenceDataGet(ADC0_BASE, 0, temp);

                /*assign sensor values to sensor variables */
                alcoholsense = temp[4];
                xval = temp[2];
                zval = temp[1];
                butval = temp[0];

                LCD_Command(CLEARSCREEN);
                LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
                LCD_PrintStringOnLCD("Xval = ");
                LCD_PrintIntOnLCD(xval);
                UARTprintf("Xval = %04d \n\n", xval);
                SysCtlDelay(SysCtlClockGet()/5);

                LCD_Command(CLEARSCREEN);
                LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
                LCD_PrintStringOnLCD("Yval = ");
                LCD_PrintIntOnLCD(yval);
                UARTprintf("Yval = %04d \n\n", yval);
                SysCtlDelay(SysCtlClockGet()/5);

                LCD_Command(CLEARSCREEN);
                LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
                LCD_PrintStringOnLCD("AlcoholVal ");
                LCD_Command(GOTO_SECONDROW_THIRDCOLUMN);
                LCD_PrintIntOnLCD(alcoholsense);
                UARTprintf("AlcoholVal = %04d \n\n", alcoholsense);
                SysCtlDelay(SysCtlClockGet()/5);

                /*calculate instataneous deviaton from reference values for head tilting */

                deltax = abs(xref - xval);
                deltaz = abs(zref - zval);

                LCD_Command(CLEARSCREEN);
                LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
                LCD_PrintStringOnLCD("Z DELTA ");
                LCD_Command(GOTO_SECONDROW_THIRDCOLUMN);
                LCD_PrintIntOnLCD(deltaz);
                UARTprintf("Delta Z = %04d \n\n", deltaz);
                SysCtlDelay(SysCtlClockGet()/5);


                LCD_Command(CLEARSCREEN);
                LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
                LCD_PrintStringOnLCD("X DELTA ");
                LCD_Command(GOTO_SECONDROW_THIRDCOLUMN);
                LCD_PrintIntOnLCD(deltax);
                UARTprintf("Delta X = %04d \n\n", deltax);
                SysCtlDelay(SysCtlClockGet()/5);

                //Check for Alcohol

                if(alcoholsense > 1000)
                {
                    alcoholdetected();
                }

                /*	Check for eyes being closed or blinking at a very
                fast rate or partially closed for long. */

                if(blinkcounter >= 12)
                {

                    LCD_Command(CLEARSCREEN);
                    LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
                    LCD_PrintStringOnLCD("Fatigued");
                    LCD_Command(GOTO_SECONDROW_THIRDCOLUMN);
                    LCD_PrintStringOnLCD("State");
                    UARTprintf("Fatigued State \n\n");
                    delay(2000);
                    alarmState();

                }

                /*Check for accelerometer alert state*/

                /*Reinitiate accelerometer Counter */
                counter = 0;

                /*If one of the values in head position differs from auto calibrated values, re-sensing is done */

                if((deltax >= 70) || (deltaz >= 70))
                {

                    for(count = 0; count <= 10; count++)
                    {

                        ADCProcessorTrigger(ADC0_BASE, 0);
                        while(!ADCIntStatus(ADC0_BASE, 0, false))
                        {
                        }

                        ADCSequenceDataGet(ADC0_BASE, 0, temp);

                        deltax = abs(xref - temp[2]);
                        deltaz = abs(zref - temp[1]);

                        if((deltax >= 70) || (deltaz >= 70))
                        {
                            counter++;
                            SysCtlDelay(SysCtlClockGet()/5);
                        }

                    }
                }

                /* Check For HEAD TILTED */

                if (counter >= 7)
                {


                    LCD_Command(CLEARSCREEN);
                    LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
                    LCD_PrintStringOnLCD("Head");
                    LCD_Command(GOTO_SECONDROW_THIRDCOLUMN);
                    LCD_PrintStringOnLCD("Tilted");
                    UARTprintf("HEAD TILT DETECTED \n\n");
                    delay(2000);
                    alarmState();

                }

                else
                {

                    LCD_Command(CLEARSCREEN);
                    LCD_Command(GOTO_FIRSTROW_THIRDCOLUMN);
                    LCD_PrintStringOnLCD("NO");
                    LCD_Command(GOTO_SECONDROW_THIRDCOLUMN);
                    LCD_PrintStringOnLCD("ALARM");
                    UARTprintf("No alarm Detected \n\n");
                    delay(2000);

               }

            }
}
