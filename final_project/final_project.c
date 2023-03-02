/********************************************
 *
 *  Name: Mantej Lamba
 *  Email: mlamba@usc.edu
 *  Section: Friday @ 2PM
 *  Assignment: Final Project
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "lcd.h"

#define BAUD 9600
#define FOSC 16000000
#define MYUBRR (FOSC / 16 / BAUD - 1)

void timer2_init();
void timer1_init();
void timer0_init();
void serial_stringout(char *str);
void rotary_init();

#define MASK_A (1 << PD3)
#define MASK_B (1 << PD2)

volatile unsigned int count = 0, rcount = 0, getCount = 0, endFinish = 0; 
volatile unsigned char change = 0, older = 0, sensing = 0;
volatile unsigned int startFirst, rcounter, speed, buzzerCount, decimalSpeed, startFirst, counter = 0; // display count
volatile unsigned char a, b, input, flagger;
volatile unsigned int pwm = 35;
volatile unsigned long result = 0, time;
volatile unsigned char new, old;
volatile unsigned char receiverChar[4];

int main(void)
{
    // Initializing Interrupts
    PCICR |= (1 << PCIE2);
    PCMSK1 |= (1 << PCINT13);
    PCMSK1 |= (1 << PCINT9);
    PCICR |= (1 << PCIE1);
    // interrupts
    sei();
    UCSR0B |= (1 << RXCIE0);
    PCMSK2 |= (1 << PCINT19);
    PCMSK1 |= (1 << PCINT10);
    PCMSK2 |= (1 << PCINT18);

    // tri state buffering
    DDRC |= (1 << PC4);
    PORTC &= ~(1 << PC4);
    lcd_init();
    timer1_init();
    rotary_init();
    timer0_init();
    timer2_init();

    // initialize the timer, ports and registers
    DDRC |= (1 << 5) | (1 << 3);
    PORTD |= (1 << 2) | (1 << 3);
    PINC &= ~(1 << 4);
    DDRB |= (1 << 4) | (1 << 5);
    DDRB |= (1 << 3);

    UCSR0B |= (1 << TXEN0 | 1 << RXEN0);
    UCSR0C = (3 << UCSZ00);
    UBRR0 = MYUBRR;

    lcd_writecommand(1);
    lcd_moveto(0, 2);
    lcd_stringout("Mantej Lamba");
    lcd_moveto(1, 2);
    lcd_stringout("EE109 Final Project");
    _delay_ms(1000);
    lcd_writecommand(1);

    int speedone, intinput, decimalinput, intfinal, decimalfinal = 0;

    PORTB &= ~(1 << 5);
    OCR2A = 11;
    _delay_ms(500);
    OCR2A = 35;
    _delay_ms(500);

    while (1)
    { // Loop forever
        // read inputs
        if (!sensing)
        {
            if (decimalSpeed < decimalfinal)
            {
                PORTB &= (1 << 4);
                PORTB |= (1 << 5);
            }
            else if (decimalSpeed > decimalfinal)
            {
                PORTB &= (1 << 5);
                PORTB |= (1 << 4);
            }
        }
        else if (sensing)
        {
            result = TCNT1;
            time = (result * 8 / 125);
            int speed = (5000)/(count);
			rcount = (5000)%(count);
			rcount /= 100;
            int intSpeed = (48 * 100 / 10);
            int decimalSpeed = ((48 * 100 % 10));
            int speedCheck = (intSpeed * 10) + decimalSpeed;
            char arr[6];
			snprintf(arr, 6, "%u%u", speed, counter);
			int i = 0;
			tx_send('[');
			while(arr[i] != '\0'){
				tx_send(arr[i]);
				i++;
			}
			tx_send(']');
            lcd_moveto(0, 3);
            char buf[20];
            snprintf(buf, 20, "%4lums   %2lu.%1lu", time, intSpeed, decimalSpeed);
            lcd_stringout(buf);
            sensing = 0;
            if ((intSpeed > count) && (count != 0))
            {
                buzzerCount = 0;
                TCCR0B |= (1 << CS02);
            }
            pwm = (((24 * speedCheck)) / 1000);
            OCR2A = 35 - pwm;
            char transmit[7];
            snprintf(transmit, 7, "[%lu%1lu]", intSpeed, decimalSpeed);
        }
        if (flagger)
        {
            TCCR1B &= ~(1 << CS12);
            TCCR1B &= ~(1 << CS10);
			time = 0;
			lcd_moveto(0,0);
			lcd_stringout("time expired    ");
			PORTC &= ~(1 << 3);
        }
        if (change)
        { // Did state change?
            change = 0;
            char buf[10];
            eeprom_update_byte((void *)100, count);
            lcd_moveto(1, 6);
            lcd_stringout(" ");
            snprintf(buf, 10, "MAX= %2d", count);
            lcd_moveto(1, 0);
            lcd_stringout(buf);
        }
        if (endFinish)
        { //receiver  read data
            endFinish = 0;
            sscanf(receiverChar, "%d", &intinput);
            speedone = (intinput * 10) + decimalinput;
        }
        if (speedone > intfinal)
        {
            PORTB |= (1 << 5);
            PORTB &= (1 << 4);
        }
        else if (speedone < intfinal)
        {
            PORTB |= (1 << 4);
            PORTB &= (1 << 5);
        }

        // then check for the decimal points
        if (rcounter == (speed % 10))
        {
            PORTB &= ~(1 << 5);
            PORTB &= ~(1 << 4);
        }
        else if (rcounter > (speed % 10))
        {
            PORTB |= (1 << 5);
            PORTB &= ~(1 << 4);
        }
        else if (rcounter < (speed % 10))
        {
            PORTB |= (1 << 4);
            PORTB &= ~(1 << 5);
        }
    }
}

void timer1_init(void)
{ // counting
    TIMSK1 |= (1 << OCIE1A);
    TCCR1B |= (1 << WGM12);
    OCR1B = 0;
}

ISR(USART_RX_vect)
{
    char input = UDR0;
    if (input == '[')
    { // start reading data
        startFirst = 1;
        endFinish = 0;
        getCount = 0;
    }
    else if (startFirst == 1)
    {
        if ((input >= '0' && input <= '9') && getCount <= 4 || (input > 92 && input < 94))
        { // stop reading end bit and add characters
            startFirst = 0;
            endFinish = 1;
        }
        else if (input == ']' && getCount > 0)
        { // add it to the buffering
            if (getCount < 4)
            {
                receiverChar[getCount] = input;
                getCount++;
            }
        }
        else
        { // garbage so we stop everything
            startFirst = 0;
        }
    }
}

void timer2_init(void)
{ // servo pwm
    TCCR2A |= (0b11 << WGM20);
    TCCR2B |= (0b111 << CS20);
    TCCR2A |= (0b10 << COM2A0);
    OCR2A = 35;
}

void timer0_init(void)
{ //  speedCheck
    TIMSK0 |= (1 << OCIE0A);
    TCCR0A |= (1 << WGM01);
    OCR0A = 80;
}

void rotary_init()
{
    unsigned int checker = PIND;
    a = checker & MASK_A;
    b = checker & MASK_B;
    input = 0;
    if (!b && !a)
        old = 0;
    else if (!b && a)
        old = 1;
    else if (b && !a)
        old = 2;
    else
        old = 3;
    new = old;
}

// rotary encoder ISR
ISR(PCINT2_vect)
{

    unsigned int get = PIND;
    b = get & MASK_B;
    a = get & MASK_A;

    if (old == 3)
    { // state 3
        if (!b)
        {
            new = 1;
            count--;
        }
        else if (!a)
        {
            new = 2;
            count++;
        }
    }
    else if (old == 0)
    {
        // state 0
        if (!a)
        {
            new = 2;
            count--;
        }
        else if (a && !b)
        {
            new = 1;
            count++;
        }
    }
    else if (old == 2)
    {

        // state 2
        if (!b && a)
        {
            new = 1;
            count--;
        }
        if (!a)
        {
            new = 0;
            count++;
        }
    }
    else if (old == 1)
    {

        // state 1
        if (!a && !b)
        {
            new = 0;
            count--;
        }

        if (a)
        {
            if (b)
            {
                new = 3;
                count++;
            }
        }
    }
    if (count > 99)
    {
        count = 99;
    }

    if (old != new)
    {
        change = 1;
        old = new;
    }

    if (count < 1)
    {
        count = 1;
    }
}

ISR(TIMER1_COMPA_vect)
{ // interrupt service
    TCCR1B &= ~((1 << CS12) | (1 << CS11));
    flagger = 1;
}

void serial_stringout(char *string)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        while ((UCSR0A & (1 << UDRE0)) == 0)
        {
        }
        UDR0 = string[i];
    }
}

ISR(TIMER0_COMPA_vect)
{ // buzzer interrupt
    PORTC ^= (1 << 5);
    buzzerCount++;
    if (buzzerCount > 375)
    {
        TCCR0B &= ~(1 << CS11) | (1 << CS10);
    }
}

// sensing interrupt
ISR(PCINT1_vect)
{
    older = count;
    if ((PINC & (1 << PC2)) == 0)
    {              // stop sensing
        count = 0; // stopped
        TCCR1B &= ~(1 << CS12);
        TCCR1B &= ~(1 << CS10);
        PORTC &= ~(1 << PC3);
        sensing = 1;
    }
    if ((PINC & (1 << PC1)) == 0)
    {              // start sensing
        count = 1; // counting
        PORTC |= (1 << PC3);
        TCCR1B |= (1 << CS12);
        TCCR1B |= (1 << CS10);
        TCNT1 = 0;
        OCR1A = 62500;
    }
}

void tx_send(char c) {
	while ((UCSR0A & (1 << UDRE0)) == 0) { }
    UDR0 = c;
}