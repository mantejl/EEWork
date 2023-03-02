/********************************************
 *
 *  Name: Mantej Lamba
 *  Email: mlamba@usc.edu
 *  Section: Friday at 2
 *  Assignment: Lab 4 - Up/Down counter on LCD display
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"

enum states
{
    UP,
    DOWN
};

int main(void)
{

    // Setup DDR and PORT bits for the 2 input buttons as necessary
    PORTC |= (1 << 2);
    PORTC |= (1 << 4);
    DDRC &= ~(1 << PC2);
    DDRC &= ~(1 << PC4);

    // Initialize the LCD
    lcd_init();

    // Use a state machine approach to organize your code
    //   - Declare and initialize a variable to track what state you
    //     are in by assigning the values UP or DOWN to that variable.
    int increments = 0;
    char state = UP;
    int check = 0;
    char count = 0;

    while (1)
    { // Loop forever
        // checking if the buttons are pressed
        if ((PINC & (1 << PC2)) == 0)
        {
            state = UP;
        }
        // checking if the buttons are pressed
        if ((PINC & (1 << PC4)) == 0)
        {
            state = DOWN;
        }
        // if down button is pressed
        if (state == DOWN)
        {
            if (check == 10)
            {
                lcd_writecommand(1);
                lcd_writedata(count + '0');
                increments--;
                if (increments == -1)
                {
                    count = 9;
                    increments = 9;
                    
                }
                check = 0;
                count = increments;         
            }
        }
        // if up button is pressed
        else if (state == UP)
        {
            // if check == 10,
            if (check == 10)
            {
                lcd_writecommand(1);
                lcd_writedata(count + '0');
                increments++;
                if (increments == 10)
                {
                    count = 0;
                    increments = 0;    
                }
                check = 0;
                count = increments;
               
            }
        }
        check++;
        _delay_ms(50);
        // Use an outer if statement to select what state you are in
        // Then inside each 'if' or 'else' block, perform the desired
        // output operations and then sample the inputs to update
        // the state for the next iteration

        // Delay before we go to the next iteration of the loop
     
    }

    return 0; /* never reached */
}
