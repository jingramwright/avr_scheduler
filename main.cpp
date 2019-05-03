/*-----------------------------------------------------------------------------
                            _______ _    _  ______      
                            |_____|  \  /  |_____/      
                            |     |   \/   |    \_   
                            
     _______ _______ _     _ _______ ______  _     _        _______  ______
     |______ |       |_____| |______ |     \ |     | |      |______ |_____/
     ______| |_____  |     | |______ |_____/ |_____| |_____ |______ |    \_
					   
--------------------------------------------------------------------------------
OVERVIEW
--------------------------------------------------------------------------------
  This implements a basic scheduler using a timer via an interrupt. Written for 
  use on the Atmega328P. It flashes 3 LED's on 1 second, 2 second, and 5
  second intervals.
--------------------------------------------------------------------------------
VERSION HISTORY
--------------------------------------------------------------------------------
  VERSION         DATE         AUTHOR                DESCRIPTION
  -------         ----         ------                -----------
  1.0             03/05/19     J. Ingram-Wright      Initial version
------------------------------------------------------------------------------*/


#include<avr/io.h>
#include<avr/interrupt.h>

#define FIRST_LED PB1
#define SECOND_LED PB2
#define THIRD_LED PB3
#define TIMER_PERIOD 1 // Desired time between interrupts.
#define TIMER_RESOLUTION 6.4e-5 // Overflow time with a 1024 prescaler.

volatile uint8_t interruptCount = 0; // Track number of interrupts for scheduling.
volatile bool    tasksExecuted  = false; // Flag to determine whether scheduled tasks have been completed.


void initialiseGpio()
{
	// Set defined pins as outputs.
	DDRB = (1 << FIRST_LED);
	DDRB = (1 << SECOND_LED);
	DDRB = (1 << THIRD_LED);
}


void initialiseTimer()
{
	// Disable global interrupts.
	cli();
	
	// Set TCCR1A and TCCR1B registers to 0.
	TCCR1A = 0;
	TCCR1B = 0;
	
	// Calculate timerCount for use in the match register.
	// NOTE: The "- 1" is to account for the clock cycle used to perform the
	//       reset to 0.
	int timerCount = (TIMER_PERIOD / TIMER_RESOLUTION) - 1.0;
	
	// Set compare match register to desired timer count.
	OCR1A = timerCount;
	// Turn on CTC mode.
	TCCR1B |= (1 << WGM12);
	// Set CS10 and CS12 bits for 1024 prescaler.
	TCCR1B |= (1 << CS10);
	TCCR1B |= (1 << CS12);
	// Enable Timer1 compare interrupt.
	TIMSK1 = (1 << OCIE1A);
	
	// Enable global interrupts.
	sei();
}


int main()
{	
	initialiseGpio();
	initialiseTimer();
	
	while(1)
	{
		// Check that the tasks have not already been executed on this cycle.
		if (!tasksExecuted)
		{
			// Scheduling case statement.
			switch (interruptCount)
			{
				case 0:
					PORTB ^= (1 << FIRST_LED);
					PORTB ^= (1 << SECOND_LED);
					PORTB ^= (1 << THIRD_LED);
					break;
				case 1:
					PORTB ^= (1 << FIRST_LED);
					break;
				case 2:
					PORTB ^= (1 << FIRST_LED);
					PORTB ^= (1 << SECOND_LED);
					break;
				case 3:
					PORTB ^= (1 << FIRST_LED);
					break;
				case 4:
					PORTB ^= (1 << FIRST_LED);
					PORTB ^= (1 << SECOND_LED);
					break;
				case 5:
					PORTB ^= (1 << FIRST_LED);
					PORTB ^= (1 << THIRD_LED);
					break;
				case 6:
					PORTB ^= (1 << FIRST_LED);
					PORTB ^= (1 << SECOND_LED);
					break;
				case 7:
					PORTB ^= (1 << FIRST_LED);
					break;
				case 8:
					PORTB ^= (1 << FIRST_LED);
					PORTB ^= (1 << SECOND_LED);
					break;
				case 9:
					PORTB ^= (1 << FIRST_LED);
					break;
			}
			// Tasks have been executed on this cycle.
			tasksExecuted = true;
		}
	}
}


ISR (TIMER1_COMPA_vect)
{
	// Increment or reset interruptCount.
	if (interruptCount < 9)
	{
		interruptCount++;
	}
	else
	{
		interruptCount = 0;	
	}
	
	// Interrupt triggered and so tasks for this cycle have not yet been
	// executed.
	tasksExecuted = false;
}