#include <avr/io.h>

#include "adc.h"


void adc_init(void)
{
    // Initialize the ADC 
    ADMUX &= ~(1<<2); 
    ADMUX &= ~(1<<3); 
    ADMUX |= (1<<6);
    ADMUX &= ~(1<<7); 
    ADCSRA |= (0x87); 
    ADCSRA &= (0x87); 
    ADMUX = (1<<5); 

}

unsigned char adc_sample(unsigned char channel)
{
    // Set ADC input mux bits to 'channel' value
    ADMUX &= ~(0x0f); 
    ADMUX |= (channel & 0x0f); 

    // Convert an analog input and return the 8-bit result
    ADCSRA |= (1<<6); 
    while ((ADCSRA &(1<<6)) != 0) {}
    unsigned char output = ADCH; 
    return ADCH; 

}
