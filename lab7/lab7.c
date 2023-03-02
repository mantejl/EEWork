/********************************************
 *
 *  Name: Mantej Lamba
 *  Email: mlamba@usc.edu
 *  Section: Friday at 2PM
 *  Assignment: Lab 7 - Rotary Encoder
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>


void variable_delay_us(int);
void play_note(unsigned short);

// Frequencies for natural notes from middle C (C4)
// up one octave to C5.
unsigned short frequency[8] =
	{262, 294, 330, 349, 392, 440, 494, 523};

volatile unsigned char new_state, old_state;
volatile unsigned char changed = 0; // Flag for state change
volatile int count = 0;			   // Count to display
volatile unsigned char a, b;

int main(void)
{
	char lcd[5];

	// Initialize DDR and PORT registers and LCD
	//lcd_init();
	PORTC |= (1 << 5) | (1 << 1);				// pull up resistors
	DDRB |= (1 << 4);							// setting output
	PCICR |= (1 << PCIE1);						// interrupts
	PCMSK1 |= ((1 << PCINT9) | (1 << PCINT13)); // PC1 and PC5
	sei();										// enable global interrupts

	// Write a spash screen to the LCD
	// lcd_writecommand(1); // clearing
	// lcd_moveto(0, 2);
	// lcd_stringout("Mantej Lamba"); // displaying name
	// lcd_moveto(1, 2);
	// lcd_stringout("Lab 7"); // displaying lab
	// _delay_ms(750);
	// lcd_writecommand(1); // clearing

	// Read the A and B inputs to determine the intial state.
	// In the state number, B is the MSB and A is the LSB.
	// Warning: Do NOT read A and B separately.  You should read BOTH inputs
	// at the same time, then determine the A and B values from that value.

	a = PINC & (1 << 1);
	b = PINC & (1 << 5);
	if (!b && !a)
		old_state = 0;
	else if (!b && a)
		old_state = 1;
	else if (b && !a)
		old_state = 2;
	else
		old_state = 3;

	new_state = old_state;

	while (1)
	{ // Loop forever
		if (changed)
		{				 // Did state change?
			changed = 0; // Reset changed flag

			// Output count to LCD
			snprintf(lcd, 5, "%3d", count);
			lcd_moveto(0, 1);
			lcd_stringout(lcd);

			// Do we play a note?
			if ((count % 8) == 0)
			{
				// Determine which note (0-7) to play
				int note = (abs(count) % 64) / 8;

				// Find the frequency of the note
				unsigned short noteFreq = frequency[note];

				// Call play_note and pass it the frequency
				play_note(noteFreq);
			}
		}
	}
}

/*
  Play a tone at the frequency specified for one second
*/
void play_note(unsigned short freq)
{
	unsigned long period;

	period = 1000000 / freq; // Period of note in microseconds

	while (freq--)
	{
		// Make PB4 high
		PORTB |= (1 << 4);

		// Use variable_delay_us to delay for half the period
		variable_delay_us(period / 2);

		// Make PB4 low
		PORTB &= ~(1 << 4);

		// Delay for half the period again
		variable_delay_us(period / 2);
	}
}

/*
	variable_delay_us - Delay a variable number of microseconds
*/
void variable_delay_us(int delay)
{
	int i = (delay + 5) / 10;

	while (i--)
		_delay_us(10);
}

ISR(PCINT1_vect) {
	// Read the input bits and determine A and B.
		unsigned char p = PINC; 
		a = p & (1 << 1);
		b = p & (1 << 5);
		// For each state, examine the two input bits to see if state
		// has changed, and if so set "new_state" to the new state,
		// and adjust the count value.
		if (old_state == 0)
		{
			// Handle A and B inputs for state 0
			if (b)
			{
				new_state = 2;
				count--;
			}
			else if (a)
			{
				count++;
				new_state = 1;
			}
		}
		else if (old_state == 1)
		{

			// Handle A and B inputs for state 1
			if (b)
			{
				new_state = 3;
				count++;
			}
			else if (!a)
			{
				new_state = 0;
				count--;
			}
		}
		else if (old_state == 2)
		{

			// Handle A and B inputs for state 2
			if (!b)
			{
				new_state = 0;
				count++;
			}
			else if (a)
			{
				new_state = 3;
				count--;
			}
		}
		else
		{ // old_state = 3

			// Handle A and B inputs for state 3
			if (!b)
			{
				new_state = 1;
				count--;
			}
			else if (!a)
			{
				new_state = 2;
				count++;
			}
		}

		// If state changed, update the value of old_state,
		// and set a flag that the state has changed.
		if (new_state != old_state)
		{
			changed = 1;
			old_state = new_state;
		}
}