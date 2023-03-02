#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "lcd.h"

volatile unsigned char new_state, old_state;
volatile unsigned char changed = 0;
// Flag for state change
volatile unsigned int PWM = 3000;
volatile int count = 128;
// Count to display
volatile unsigned char a, b, input;
// void timer1_init();
// void timer2_init();

int main(void)
{
	timer1_init();
	timer2_init();

	lcd_init();

	PORTC |= (1 << PC4);
	PORTC |= (1 << PC5);

	DDRB |= (1 << 3);
	DDRB |= (1 << 2);

	// Initializing Interrupts
	PCMSK1 |= ((1 << PCINT12) | (1 << PCINT13));
	PCICR |= (1 << PCIE1);

	input = PINC;
	a = input & (1 << 4);
	b = input & (1 << 5);
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
	{
		// Loop forever
		// Read the input bits and determine A and B.
		// read inputs
		if (changed)
		{ // Did state change?
			changed = 0;
			snprintf(lcd, 5, "%3d", count);
			lcd_moveto(0, 1);
			lcd_stringout(lcd);
			OCR2A = count;
			OCR1B = PWM;
		}
	}
}

void timer1_init(void)
{
	TCCR1A |= (1 << COM1B1);
	TIMSK1 |= (1 << TOIE1);
	TCCR1B |= (0b11 << WGM12);
	TCCR1A |= (0b11 << WGM10);
	OCR1A = 40000;
	OCR1B = 3000;
	TCCR1B |= (1 << CS11);
}

void timer2_init(void)
{
	TCCR2A |= (0b11 << WGM00);
	// Fast PWM mode, modulus = 256
	TCCR2A |= (0b10 << COM0A0); // Turn D11 on at 0x00 and off at OCR2A
	OCR2A = 128;
	// Initial pulse duty cycle of 50%
	TCCR2B |= (0b111 << CS20);
	// Prescaler = 1024 for 16ms period
}

ISR(PCINT1_vect)
{
	input = PINC;
	a = input & (1 << 4);
	b = input & (1 << 5);
	if (old_state == 2)
	{
		// Handle A and B inputs for state 2
		if (!a && b)
		{
			new_state = 3;
			count += 5;
			PWM += 50;
		}
		else if (!b && a)
		{
			new_state = 1;
			count -= 5;
			PWM -= 50;
		}
	}
	else if (old_state == 1)
	{
		if (!a && !b)
		{
			new_state = 0;
			count -= 5;
			PWM -= 50;
		}else if (a && b)
		{
			new_state = 2;
			count += 5;
			PWM += 50;
		}
		
	}
	else if (old_state == 0)
	{
		if (!a && b)
		{
			new_state = 3;
			count -= 5;
			PWM -= 50;
		}
		else if (a && !b)
		{
			new_state = 1;
			count += 5;
			PWM += 50;
		}
	}
	else if (old_state == 3)
	{
		if (!a && !b)
		{
			new_state = 0;
			count += 5;
			PWM += 50;
		} else if (a && b)
		{
			new_state = 2;
			count -= 5;
			PWM -= 50;
		}
	}

	if (count > 255)
	{
		count = 255;
	}
	else if (count < 0)
	{
		count = 0;
	}

	if (PWM > 4500)
	{
		PWM = 4500;
	}
	else if (PWM < 1500)
	{
		PWM = 1500;
	}
	

	if (new_state != old_state)
	{
		changed = 1;
		old_state = new_state;
	}
}
