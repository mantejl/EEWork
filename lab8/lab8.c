/********************************************
 *
 *  Name: Mantej Lamba
 *  Email: mlamba@usc.edu
 *  Section: Friday @ 2PM
 *  Assignment: Lab 8 - Digital Stopwatch
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "adc.h"

void timer_init(); 
volatile unsigned char ten = 0; 
volatile unsigned char one = 0; 
volatile unsigned char tenth = 0; 
volatile unsigned char display;

int main(void) {
    lcd_init(); // initializations
    adc_init();
    timer_init();
    unsigned char adc; // adc sample
    unsigned char state = 0; 
    char clock[10]; 

    lcd_writecommand(1); // splash screen 
    lcd_moveto(0,1); 
    lcd_stringout("Mantej Lamba");
    lcd_moveto(1,1); 
    lcd_stringout("Lab 8"); 
    _delay_ms(500); 
    lcd_writecommand(1);
    lcd_moveto(1,0);
    lcd_stringout("0.00");  

    while (1) {
        adc = adc_sample(0); 
        if (state == 1) { // start 
            display = 1;  // only change when start or lap 
            if(adc <= 60 && adc >= 50) {
                state = 0; 
                TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12)); // stop clock
                _delay_ms(200);  
            } else if (adc <= 110 && adc >= 100) {
                state = 2; 
                _delay_ms(200); 
            }
        } else if (state == 2) { // lap 
            if ((adc <= 60 && adc >= 50) || (adc <= 110 && adc >= 100)) { // can only go to start 
                state = 1; 
                _delay_ms(200); 
            }
        } else if (state == 0) { // stopped 
            if (adc <= 60 && adc >= 50) {
                state = 1; 
                TCCR1B |= (1 << CS10) | (1 << CS11);
                _delay_ms(200); 
            } else if (adc <= 110 && adc >= 100) {
                tenth = 0; 
                one = 0; 
                ten = 0; 
                display = 1; // only change when start or lap 
                _delay_ms(200); 
            }
        }
        if (display == 1) { // changing display
            display = 0; // setting to 0 
            snprintf(clock,10,"%d%d.%d", ten, one, tenth);  // creating string   
            lcd_moveto(1,0); 
            lcd_stringout(clock);  // printing 
        }
    }
    return 0; 
}

void timer_init() {
    TCCR1B |= (1 << WGM12); // set to CTC mode
    TIMSK1 |= (1<< OCIE1A); // enable timer 
    OCR1A = 25000; // load count 
    sei(); // global interrupts 
}

ISR(TIMER1_COMPA_vect) {
    tenth++; // 0.1 seconds have passed no matter what 
    if (tenth > 9) { // 1 second has passed 
        tenth = 0;
        one++; 
        if (one > 9) { // 10 seconds have passed
            one = 0; 
            ten++; 
        }
    }
    if(ten == 6) { // resetting digits 
        ten = 0; 
        one = 0; 
        tenth = 0; 
    }
}
