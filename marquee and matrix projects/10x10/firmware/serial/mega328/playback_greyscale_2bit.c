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

============================================================================*/

#include <avr/pgmspace.h>
#include <avr/io.h>
#define N 10
#include "video.h"

uint32_t v_bit_index = 0;
v_type video_data  = 0;
uint8_t  nextBit()
{
    if (!(v_bit_index&7))
        video_data = VIDEO(v_bit_index>>3);
    uint8_t bit = video_data&1;
    video_data>>=1;
    v_bit_index++;
    if (v_bit_index>=(uint32_t)VIDEO_LENGTH_BITS)
    {
        //v_bit_index--;
        //return 0;
        v_bit_index = 0;
    }
    return bit;
}

uint8_t  pin_numbers[11] = {0,1,2,6,7,10,11,12,13,14,15};
uint32_t getPinstate(uint32_t vector)
{
	uint32_t value = 0;
	uint8_t i;
	for (i=0;i<11;i++) {
		if (vector&1) value |= 1<<pin_numbers[i];
		vector>>=1;
	}
	return value;
}

int main()
{	
    UCSR0B = 0x18;
    UCSR0C = 0x86;
    UBRR0L = 0x33;
    uint32_t display_memory[(N+1)*2];
    uint32_t *disp = &display_memory[0];
    uint32_t *buff = &display_memory[N+1];
    uint32_t *temp;
    uint32_t pin=0, ddr=0, rx_n=0, mask=0;
    uint8_t  rx_i=0, d=0, f=0, i=0, j=0, k=0, x=0, w_i=0, r_int=0;
    uint8_t  bits_loaded =0;
    uint8_t  words_loaded=0;
    uint32_t word_loaded =0;

    while(1) 
    for (d=0   ; d<3 ; d++) // iterate over display bits
    {
        for (k=1<<d; k>0 ; k--) // iterate over display bits
        {
            for (i=0; i<=N; i++) // scan display
            {
                pin = 1<<i;
                ddr = disp[i];
                ddr = (i!=N)? ((ddr>>9*d)&0x1ff) : ((ddr>>10*d)&0x3ff);
                mask = (1<<i)-1;
                ddr = (ddr&mask)|((ddr&~mask)<<1);
                pin = getPinstate(1<<i);
                ddr = getPinstate(ddr|1<<i);
                if (i!=N) pin=~pin;
                pin &= ddr;
                DDRC  =      DDRB  =         DDRD  = 0;
                PORTB = pin; PORTD = pin>>8; PORTC = pin>>16; 
                DDRB  = ddr; DDRD  = ddr>>8; DDRC  = ddr>>16;
            }
        }
        
        for (i=0; i<=5; i++) // LOAD SOME VIDEO DATA
        {
            word_loaded |= (uint32_t)nextBit() << bits_loaded;
            uint8_t blen = words_loaded==10?10:9;
            if (++bits_loaded==blen*2)
            {
                word_loaded |= (word_loaded<<blen*2)&(word_loaded<<blen)|(word_loaded>>blen);
                buff[words_loaded] = word_loaded;
                if (++words_loaded==11)
                {
                    words_loaded=0;
                    temp=buff; 
                    buff=disp; 
                    disp=temp;
                }
                bits_loaded = 0;
                word_loaded = 0;
            }
        }
    }
} 










