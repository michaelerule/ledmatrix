/*
Hardware notes
============================================================================

    DDRx   : 1 = output, 0 = input
    PORTx : output buffer
    PINx  : digital input buffer ( writes set pullups )
    Port B is the chip IO port ( other ports are control registers )

        AtTiny2313 pinout:
                   __ __
         RESET A2-|  U  |-VCC
            RX D0-|     |-B7 SCK
            TX D1-|     |-B6 MISO
               A1-|     |-B5 MOSI
               A0-|     |-B4
               D2-|     |-B3
               D3-|     |-B2
               D4-|     |-B1
               D5-|     |-B0
              GND-|_____|-D6
                
        Programmer pinout, 6 pin:
                
        6 MISO +-+  VCC 3
        5 SCK  + + MOSI 2 
        4 RST  +-+  GND 1
        
        Programmer pinout, 10 pin:
                
        3 vcc  +-+   MOSI 2
               + +    
               + +]  RST  4 
               + +   SCK  5 
        1 gnd  +-+   MISO 6
        
    PORT : write to here to set output
    DDR  : write to here to set IO. 1 for output.
    PIN  : pull ups ? digital input ?

thanks to http://brownsofa.org/blog/archives/215 for explaining timer interrupts

run this to make it go 8x as fast : 
avrdude -c avrispmkII -p m328p -B100 -P /dev/ttyUSB0 -U lfuse:w:0xE2:m 

run this to change it back :
avrdude -c avrispmkII -p m328p -B100 -P /dev/ttyUSB0 -U lfuse:w:0x62:m 

to check serial,
-- set up to listen
-- listen in screen
-- set firmware to scan baudrate
-- wait for something sane to appear in screen
-- exit
-- use this baudrate

stty -F /dev/ttyUSB1 9600 cs8 cread
screen /dev/ttyUSB1
C-a : quit

============================================================================
*/
#include <avr/io.h>
#include <avr/interrupt.h>

#define SCANRATE 28

#include "serial.h"

ISR(TIMER1_COMPA_vect) {
    handle_serial();
}

void timer1(void) 
{ 
    OCR1A = SCANRATE; 
    TCCR1A = 0x00;   
    TCCR1B = (0 << WGM13)|(1 << WGM12)|(0 << CS12)|(1 << CS11)|(1<< CS10);
    TIMSK |= (1 << OCIE1A);  
    sei();  
} 

int main (void)
{
    InitUART();
    timer1();
    while (1);
}

