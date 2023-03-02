/********************************************
*
*  Name: Mantej Lamba
*  Email:
*  Lab section:
*  Assignment: Lab 3 - Arduino Input and Output
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>

#define DOT_LENGTH  250         /* Define the length of a "dot" time in msec */

void dot(void);
void dash(void);
void makeOutput(char);
char checkInput(char);

int main(void)
{

    // Initialize appropriate DDR registers
    DDRD |= (1 << PD2); 
    // Initialize the LED output to 0
    PORTD &= ~(1 << PD2); 
    // Enable the pull-up resistors for the 3 button inputs 
    PORTB |= (1 << 5);
    PORTB |= (1<< 4); 
    PORTB |= (1 << 3);  

    // Loop forever
    while (1) {   
    
      // checking if button C is being pressed
      if (checkInput(5) == 1) {
        dash(); 
        dot();
        dash(); 
        dot();
        _delay_ms(750);
      } 
      
      // checking is button B is being pressed
      if (checkInput(4) == 1) {
        dot(); 
        dot();
        dot(); 
        _delay_ms(750);
      } 
      
      // checking if button A is being pressed
      if (checkInput(3) == 1) {
        dot(); 
        dot();
        dash();
        _delay_ms(750); 
      }
       

    }

    return 0;   /* never reached */
}

/*
  dot() - Makes the output LED blink a "dot".

  Write code to generate a dot by using the makeOutput function
  to turn the output ON and OFF with appropriate delays.
  Be sure you don't forget to also delay an
  appropriate time after you turn the output off.
*/
void dot()
{
  makeOutput(1);
  _delay_ms(250); 
  makeOutput(0); 
  _delay_ms(250);
}

/*
  dash() - Makes the output LED blink a "dash".

  Write code to generate a dash by using the makeOutput function
  to turn the output ON and OFF with appropriate delays.
  Be sure you don't forget to also delay an
  appropriate time after you turn the output off.
*/
void dash()
{
  makeOutput(1);
  _delay_ms(750);
  makeOutput(0); 
  _delay_ms(250);
}

/*
  makeOutput() - Changes the output bit (Group D, bit 2) to either
  a zero or one, based on the input argument "value".
  
  If the argument is zero, turn the output OFF,
  otherwise turn the output ON. 
  
  Do not use any delays here.  Just use bit-wise operations
  to make the appropriate PORT bit turn on or off.
*/
void makeOutput(char value)
{
  if (value != 0) {
     PORTD |= (1 << 2);
  } else {
    PORTD &= ~ (1 << 2); 
  }
}

/*
  checkInput(bit) - Checks the state of the input bit in Group B specified by
  the "bit" argument (0-7), and returns either 0 or 1 depending on its state.
  
  Write code to use the appropriate group's PIN register and determine if  
  the specified bit (which is passed as the argument) of that group is
  pressed or not.  Think carefully about what bit value means "pressed"
  when connected to a pushbutton.
 */
char checkInput(char bit)
{
  if (PINB & (1 << bit)) {
    return 0; 
  } else {
    return 1; 
  }
}
