/*
Hardware notes
===============================================================================

DDRx   : 1 = output, 0 = input
PORTx : output buffer
PINx  : digital input buffer ( writes set pullups )

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
        
        Programmer pinout, 6 pin, linear:
                
        6 MISO +  
        5 SCK  + 
        4 RST  +  
        VCC 3  +
        MOSI 2 +
        GND 1  +

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
avrdude -c avrispmkII -p m328p -B1000 -P /dev/ttyUSB0 -F

0 a0
1 c3
2 c5
3 a3
4 c0
5 a1
6 c6
7 c7

8 a2
9 a5
A c4
B a7
C c2
D c1
E x 
F x

G a6
H a4

4 c0
D c1
C c2
1 c3
A c4
2 c5
6 c6
7 c7

0 a0
5 a1
8 a2
3 a3
F a4
9 a5
E a6
B a7

0 a0
5 a1
8 a2
3 a3
H a4
9 a5
G a6
B a7

4 c0
D c1
C c2
1 c3
A c4
2 c5
6 c6
7 c7

E x 
F x


============================================================================*/

#include <avr/io.h>
#include <avr/interrupt.h>
#define N 8
#define NOP __asm__("nop\n\t")

uint8_t anodes[N]   = {0,5,16, 3,8,17, 9,12};
uint8_t cathodes[N] = {7,6, 2,13,1,11,10, 4};
uint8_t anodes2[N]  = {7,6, 11,13,1,2,10, 4};

void delay(uint32_t n)
{
    while (n--) NOP;
}

void setDDR(uint32_t ddr) 
{
    DDRD = ddr & 0xff;
    ddr >>= 8;
    DDRC = ddr & 0xff;//3f;
    ddr >>= 8;
    DDRB = ddr & 0xff;//c7;
}

void setPort(uint32_t pins)
{
    PORTD = pins & 0xff;
    pins >>= 8;
    PORTC = pins & 0xff;//3f;
    pins >>= 8;
    PORTB = pins & 0xff;//c7;
}

ISR(TIMER0_COMPA_vect) 
{
	;
}

ISR(TIMER1_COMPA_vect) 
{
	;
}

int main()
{    
    uint32_t i,r,c,j;
    uint32_t ii = 0;

    TIMSK0 = 2; // Timer CompA interupt 1
    TCCR0B = 1; // speed
    TCCR0A = 2; // CTC mode
    OCR0A  = 0; // period
    
    TIMSK1 = 2; // Timer CompA interupt 2
    TCCR1B = 1; // speed
    TCCR1A = 2; // CTC mode
    OCR1A  = 0; // period
    
    sei();
	
	DDRB=DDRC=DDRD=0xff;
	while(1) {
		for (r=0;r<N;r++) {
			i = anodes[r];
			setPort(((uint32_t)1)<<i);
			delay(1000);
		}
		/*
		for (r=0;r<N;r++) {
			i = anodes2[r];
			setPort(~((uint32_t)1)<<i);
			delay(10000);
		}*//
	}
} 

