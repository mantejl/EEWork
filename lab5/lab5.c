/********************************************
*
*  Name: Mantej Lamba
*  Email: mlamba@usc.edu
*  Section: Friday at 2PM
*  Assignment: Lab 5 - Combination Lock
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "lcd.h"

enum states { S0, S1, S2, S3, S4, S5 };

int main(void) {

    PORTC |= (1 << 2);
    PORTC |= (1 << 4);
    DDRC &= ~(1 << PC2);
    DDRC &= ~(1 << PC4);
    char state = S0;    
    int time = 0; 
    char *status = "Locked"; 
    char *output = ""; 

    /* Call lcd_init to set up the LCD */
    lcd_init(); 

    /* Call lcd_stringout to print out your name */
    lcd_writecommand(1);  
    lcd_moveto(0,2); 
    lcd_stringout("Mantej Lamba");

    /* Use snprintf to create a string with your birthdate */
    char birthday[9];
    unsigned char day, month, year; 
    day = 21;
    month = 12;
    year = 02; 
    snprintf(birthday,9,"%d/%d/%02d", month, day, year); 

    /* Use lcd_moveto to start at an appropriate column 
       in the bottom row to appear centered */
    lcd_moveto(1,2); 

    /* Use lcd_stringout to print the birthdate string */
    
    //lcd_stringout(birthday); 
    lcd_stringout("EE109 Lab 5"); 

    /* Delay 1 second */
    _delay_ms(1000); 
    lcd_writecommand(1); 

    while(1) { 
        // checking if button B is pressed 
        if((PINC & (1 << PC2)) == 0) {
            // debouncing 
            _delay_ms(5);
            while((PINC & (1 << PC2)) == 0) {
            }
            _delay_ms(5); 
            // doing the state transitions
            if (state == S0) {
                time = 0; 
                state = S1; 
                output = "B"; 
            } else if (state == S2) {
                time = 0;
                state = S3; 
                output = "BAB"; 
            } else if (state == S3) {
                time = 0;
                state = S4; 
                output = "BABB"; 
            } else if (state == S1) {
                time = 0;
                state = S1; 
            } else if (state == S4) {
                time = 0;
                state = S1; 
                output = "B";
            } else if (state == S5) {
                time = 0; 
                state = S0; 
                output = "";
                status = "Locked"; 
            }
            // checking if button A is pressed
        } else if ((PINC & (1 << PC4)) == 0) {
            // debouncing 
            _delay_ms(5);
            while((PINC & (1 << PC4)) == 0) {
            }
            _delay_ms(5); 
            // doing the state transitions
            if (state == S1) {
                time = 0;
                state = S2; 
                output = "BA"; 
            } else if (state == S4) {
                time = 0;
                state = S5; 
                output = "BABBA"; 
                status = "Unlocked"; 
            } else if (state == S2) {
                time = 0;
                state = S0; 
                output = "";
            } else if (state == S3) {
                time = 0;
                state = S2; 
                output = "BA";
            } else if (state == S0) {
                time = 0;
                state = S0; 
            } else if (state == S5) {
                time = 0; 
                state = S0; 
                output = "";
                status = "Locked"; 
            }
        } 
        // delaying 
        _delay_ms(50); 
        // incrementing the time counter
        time++;
        // if the time elapses 3 seconds then reset back to S0
        if (time == 60) {
            state = S0; 
            output = ""; 
            status = "Locked"; 
            time = 0; 
        }
        // writing out the information on the LCD
        lcd_writecommand(1); 
        lcd_moveto(0,2); 
        lcd_stringout(output); 
        lcd_moveto(1,2); 
        lcd_stringout(status); 
    }

    return 0;   /* never reached */
}
