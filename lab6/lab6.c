/********************************************
 *
 *  Name: Mantej Lamba
 *  Email: mlamba@usc.edu
 *  Section: Friday @ 2PM
 *  Assignment: Lab 6 - Analog-to-digital conversion
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lcd.h"
#include "adc.h"

void rand_init(void);

int main(void)
{
    int count = 0;
    unsigned int check;
    // Initialize the LCD
    lcd_init();


    // Initialize the ADC
    adc_init(); 


    // Initialize the random number function
    rand_init();


    // Write splash screen
    lcd_writecommand(1);
    lcd_moveto(0,3); 
	lcd_stringout("Mantej Lamba");
	lcd_moveto(1, 3);
	lcd_stringout("EE109 Lab 6");
	_delay_ms(500);

    // Find a random initial position of the 'X'
    int position = rand(); 
    position = position % 16; 


    // Display the 'X' on the screen
    lcd_writecommand(1);
    lcd_moveto(0,position); 
    lcd_stringout("X"); 


    while (1) {                 // Loop forever
    check = adc_sample(0); 
	// Check for button pressed
    if (check < 252) {
        if (check >= 0 && check <= 22) {
            if(position < 15) {
                lcd_moveto(0, position);
				lcd_stringout(" ");
				lcd_moveto(0, position+1);
				lcd_stringout("X");
				position = position + 1;
				count = 0;
			}
            _delay_ms(75);
        }
        else if(check >= 145 && check <= 165){
            if (position > 0) {
                lcd_moveto(0, position);
				lcd_stringout(" ");
				lcd_moveto(0, position-1);
				lcd_stringout("X");
				position = position - 1;
				count = 0;
            }
            _delay_ms(75);
        }
    }


        // If pressed, move 'X' to new position


        // Do a conversion of potentiometer input
        unsigned char potent = adc_sample(3); 
        potent = potent / 16; 


	// Move '^' to new position
    lcd_moveto(1,0);
    lcd_stringout("                  "); 
    lcd_moveto(1,potent);
    lcd_stringout("^"); 


	// Delay
    _delay_ms(10);


	// Check if '^' is aligned with 'X'
    if (potent == position) {
        count ++; 
        if(count == 75) {
            lcd_moveto(0,1);
            lcd_stringout("   You Win!   ");
            break; 
        }
    }

}

    return 0;   /* never reached */
}

void rand_init()
{
    int seed = 0;
    unsigned char i, j, x;

    // Build a 15-bit number from the LSBs of an ADC
    // conversion of the channels 1-5, 3 times each
    for (i = 0; i < 3; i++) {
	for (j = 1; j < 6; j++) {
	    x = adc_sample(j);
	    x &= 1;	// Get the LSB of the result
	    seed = (seed << 1) + x; // Build up the 15-bit result
	}
    }
    srand(seed);	// Seed the rand function
}
