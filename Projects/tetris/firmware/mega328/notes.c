/*
Hardware notes
===============================================================================

    DDRx   : 1 = output, 0 = input
    PORTx : output buffer
    PINx  : digital input buffer ( writes set pullups )

( colors are for custome "ugly pin" adaptor )
                          __ __
         !RESET     PC6 -|  U  |- PC5
                    PD0 -|     |- PC4
                    PD1 -|     |- PC3
                    PD2 -|     |- PC2
                    PD3 -|  m  |- PC1
                    PD4 -|  3  |- PC0
                    VCC -|  2  |- GND
                    GND -|  8  |- AREF
                    PB6 -|     |- AVCC
                    PB7 -|     |- PB5   SCK  ( yello )
                    PD5 -|     |- PB4   MISO ( green )
                    PD6 -|     |- PB3   MOSI ( blue )
                    PD7 -|     |- PB2
                    PB0 -|_____|- PB1

        Programmer pinout, 6 pin:
                
        6 MISO +-+  VCC 3
        5 SCK  + + MOSI 2 
        4 RST  +-+  GND 1
        Programmer pinout, 6 pin:
                
        6 MISO +-+  
        5 SCK  + + 
        4 RST  +-+  
        VCC 3
        MOSI 2
        GND 1

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

avrdude -c avrispmkII -p m328p -B100 -P /dev/ttyUSB0 -U lfuse:w:0xE2:m -U hfuse:w:0xD9:m -U efuse:w:0xFF:m -F
avrdude -c avrispmkII -p m328p -B100 -P /dev/ttyUSB0 -U lfuse:w:0xE2:m 


1350

avrdude -c avrispmkII -p m328p -B1000 -P /dev/ttyUSB0 -F

=============================================================================*/

#define NOP __asm__("nop\n\t")
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "scale.c"

/*
#define NNOTES 92
uint16_t notes[NNOTES] = {339, 269, 240, 269, 339, 359, 339, 269, 40, 480, 359, 339, 403, 33, 40, 44, 60, 67, 60, 44, 40, 44, 60, 67, 84, 339, 269, 240, 269, 339, 359, 339, 269, 40, 480, 359, 339, 403, 33, 40, 44, 60, 67, 60, 40, 60, 403, 44, 33, 40, 240, 403, 339, 40, 403, 33, 40, 44, 50, 44, 40, 33, 40, 44, 40, 44, 60, 67, 40, 60, 403, 44, 33, 40, 240, 403, 339, 40, 403, 33, 40, 44, 50, 44, 40, 33, 40, 44, 40, 44, 60, 67};
*/


void setDDR(uint32_t ddr) 
{
    DDRB = ddr;
    ddr >>= 8;
    DDRD = ddr;
    ddr >>= 8;
    DDRC = ddr;
}

void setPort(uint32_t pins)
{
    PORTB = pins;
    pins >>= 8;
    PORTD = pins;
    pins >>= 8;
    PORTC = pins;
}

volatile uint8_t speaker = 0;
volatile uint32_t speaker_delay = 100;
volatile uint32_t note = 0;
volatile uint32_t noteindex = 0;
volatile uint32_t notecounter = 0;

void nextNote()
{
	noteindex = (noteindex+1)%NNOTES;
	uint16_t info = pgm_read_word_near(&unique[pgm_read_byte_near(&notes[noteindex])]);
	note = pgm_read_byte_near(&pitches[info&0xff]);
	notecounter = 4000*(info>>8);
}

ISR(TIMER0_COMPA_vect) 
{
	if (speaker_delay) speaker_delay--;
	else {
		PORTB=PORTD=PORTC=speaker?0:0xff;
   		speaker = !speaker;
   		speaker_delay = speaker?10:note-10;
   	}
   	if (notecounter) notecounter--;
   	else nextNote();
}

void delay(uint32_t n)
{
    while (n--) NOP;
}

#define K 50
int main()
{    
    uint16_t i,r,c,j;
    
    setDDR(0xffffffff);
    
    TIMSK0 = 2; // Timer CompA interupt
    TCCR0B = 1; // speed
    TCCR0A = 2; // CTC mode
    OCR0A  = 0; // period
    sei();

	while(1);
} 

