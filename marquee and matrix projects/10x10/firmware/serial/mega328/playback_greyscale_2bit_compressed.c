/*
Hardware notes
============================================================================

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

avrdude -c avrispmkII -F -p m328p -B100 -P /dev/ttyUSB0 -U lfuse:w:0xE2:m 
1350

avrdude -c avrispmkII -p m328p -B1000 -P /dev/ttyUSB0 -F

x876543210 x876543210 x876543210
9x76543210 9x76543210 9x76543210
98x6543210 98x6543210 98x6543210
987x543210 987x543210 987x543210
9876x43210 9876x43210 9876x43210
98765x3210 98765x3210 98765x3210
987654x210 987654x210 987654x210
9876543x10 9876543x10 9876543x10
98765432x0 98765432x0 98765432x0
987654321x 987654321x 987654321x
9876543210 9876543210 9876543210


stty -F /dev/ttyUSB1 9600 cs8 cread

============================================================================*/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#define N 10
#include "video.h"
#define SCANINTERVAL 220
#define NOP __asm__("nop\n\t")
#define once(condition) {while(!(condition));}
#define DATA_AVAILABLE (UCSR0A&(1<<RXC0))
#define OK_TO_SEND (UCSR0A & (1 << UDRE0))

void send(uint8_t data )
{
    while (!OK_TO_SEND);
    UDR0 = data;
}

volatile uint32_t time = 0;
uint8_t  frame[100];
uint32_t display_memory[(N+1)*2];
uint32_t *disp = &display_memory[0];
uint32_t *buff = &display_memory[N+1];
uint32_t *temp;
uint32_t v_bit_index = 0;
uint8_t  video_data  = 0;

uint8_t  nextBit()
{
    if (!(v_bit_index&7))
        video_data = VIDEO(v_bit_index>>3);
    uint8_t bit = video_data&1;
    video_data>>=1;
    v_bit_index++;
    if (v_bit_index>=(uint32_t)VIDEO_LENGTH_BITS)
        v_bit_index = 0;
    return bit;
}

uint8_t  pin_numbers[11] = {0,1,2,6,7,10,11,12,13,14,15};

uint32_t getPinstate(uint32_t vector)
{
	uint32_t value = 0;
	uint8_t i;
	for (i=0;i<11;i++) 
	{
		if (vector&1) value |= 1<<pin_numbers[i];
		vector>>=1;
	}
	return value;
}

uint8_t scanD=0, scanK=0, scanI=0;

ISR(TIMER0_COMPA_vect) 
{
    time++;
    
    if (++scanI>N) {
        scanI=0;
        if (++scanK>=(1<<scanD)) { 
            scanK=0;
            if (++scanD>=3) scanD=0;
        }
    }
    
    uint32_t pin, ddr, msk;
    
    pin = 1<<scanI;
    ddr = disp[scanI];
    ddr = (scanI!=N)? ((ddr>>9*scanD)&0x1ff) : ((ddr>>10*scanD)&0x3ff);
    msk = (1<<scanI)-1;
    ddr = ddr&msk | ((ddr&~msk)<<1);
    pin = getPinstate(1<<scanI);
    ddr = getPinstate(ddr)|pin;
    
    if (scanI != N) pin=~pin;
    pin &= ddr;
    
    DDRC  =      DDRB  =         DDRD  = 0;
    PORTB = pin; PORTD = pin>>8; PORTC = pin>>16; 
    DDRB  = ddr; DDRD  = ddr>>8; DDRC  = ddr>>16;
}

int main()
{	 
    UCSR0B = 0x18;
    UCSR0C = 0x86;
    UBRR0L = 0x33;
    TIMSK0 = 2;            // Timer CompA interupt
    TCCR0B = 2;            // speed
    TCCR0A = 2;            // CTC mode
    OCR0A  = SCANINTERVAL; // period
    sei();

    uint8_t i,j;
    uint16_t fi;

    while (1)
    {
        
        v_bit_index = 0;
        for (i=0; i<25; i++) 
            for (j=0; j<4; j++) 
                frame[i*4+j] = (KEYFRAME(i)>>2*j) & 3;
    
    
        for(fi=0; fi<VIDEO_LENGTH_FRAMES; fi++)
        {
            time = (uint32_t)0;
            uint8_t index = -1;
            while (1)
            {
                uint8_t event = 0;
                while (1)
                {
                    if (nextBit())
                    {
                        uint8_t x = 0;
                        x|=nextBit()<<2;
                        x|=nextBit()<<1;
                        x|=nextBit()<<0;
                        event += x;
                        break;
                    }
                    event+=8;
                }
                if (!event) break;
                index += (uint8_t)(event>>1);
                frame[index] += (event&1)?(int8_t)1:(int8_t)-1;
            }
            
            for (i=0;i<10;i++)
            {
                uint32_t rowdata=0;
                for (j=0; j<10; j++)
                {
                    uint8_t pixel = frame[i*10+j];
                    if (i!=j)
                    {
                        uint8_t J = j>=i? j-1:j;
                        rowdata |= (uint32_t)(pixel>0?1:0) << J;
                        rowdata |= (uint32_t)(pixel>1?1:0) << J+9;
                        rowdata |= (uint32_t)(pixel>2?1:0) << J+18;
                    }
                }
                buff[i]=rowdata;
            }    
            uint32_t diagdata=0;
            for (i=0;i<10;i++)
            {
                uint8_t pixel = frame[i*10+i];
                diagdata |= (uint32_t)(pixel>0?1:0) << i;
                diagdata |= (uint32_t)(pixel>1?1:0) << i+10;
                diagdata |= (uint32_t)(pixel>2?1:0) << i+20;
            }    
            buff[i]=diagdata;
                
            temp=buff; buff=disp; disp=temp;
            while (time<(uint32_t)300);
        }
    }
}







