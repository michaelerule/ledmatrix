
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h> 

// Global variables

float vcc;//variable to hold the value of Vcc

void setup_adc(void)
{
    ADMUX = 0xE; //Set the Band Gap voltage as the ADC input
    ADCSRA = (1<<ADEN)|(1<<ADATE)|(1<<ADIE)|(1<<ADSC)|5;
}

ISR(ADC_vect) //ADC End of Conversion interrupt 
{
unsigned char adc_data;
adc_data = ADC>>2; //read 8 bit value
vcc = 1.1 * 255 / adc_data;
}

// ***********************************************************
// Main program
// ***********************************************************
int main(void)
{
    DDRB = DDRB | (1<<PB0); //set PB0 as output (for the LED).
    sei(); //Activate interrupts

    setup_adc(); //setup the ADC

    while(1) { // Infinite loop
        if (vcc < 3.2)
        {
            PORTB |= (1<<PB0);
        }
        else
        {
            PORTB &= ~(1<<PB0);
        }
    }
}


