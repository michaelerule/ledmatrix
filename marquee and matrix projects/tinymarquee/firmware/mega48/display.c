/*
Hardware notes
===============================================================================

DDRx   : 1 = output, 0 = input
PORTx : output buffer
PINx  : digital input buffer ( writes set pullups )
                          ______
         !RESET     PC6 -|  U  |- PC5
                    PD0 -|     |- PC4
                    PD1 -|     |- PC3
                    PD2 -|     |- PC2
                    PD3 -|  m  |- PC1
                    PD4 -|  *  |- PC0
                    VCC -|  8  |- GND
                    GND -|     |- AREF
                    PB6 -|     |- AVCC
                    PB7 -|     |- PB5   SCK  ( yellow )
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

avrdude -c avrispmkII -p m48 -B100 -P /dev/ttyUSB0 -U lfuse:w:0xE2:m 
avrdude -c avrispmkII -p m48 -B1000 -P /dev/ttyUSB0 -F


#./blues.py | tee blues.c &&\
#./scale.py | tee scale.c &&\
#./chromatic_up.py | tee chromatic_up.c &&\
#./chromatic_down.py | tee chromatic_down.c &&\
avr-gcc -Os -mmcu=atmega48 ./display.c -o a.o &&\
avr-objcopy -j .text -j .data -O binary a.o a.bin  && du -b ./a.bin &&\
avrdude -F -c avrispmkII -p m48 -B1 -P /dev/ttyUSB0 -F -U flash:w:a.bin


*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define N 8
#define ONE ((uint32_t)1)
//SERIAL MACROS
#define DATA_AVAILABLE (UCSR0A&(1<<RXC0))
#define OK_TO_SEND (UCSR0A & (1 << UDRE0))
#define NOP __asm__("nop\n\t")
#define once(condition) {while(!(condition));}

//#include "alphabet.c"

void setDDR(uint32_t ddr) 
{
    DDRD = ddr & 0xff;
    ddr >>= 8;
    DDRC = ddr & 0xff;
    ddr >>= 8;
    DDRB = ddr & 0xff;
}

void setPort(uint32_t pins)
{
    PORTD = pins & 0xff;
    pins >>= 8;
    PORTC = pins & 0xff;
    pins >>= 8;
    PORTB = pins & 0xff;
}

void delay(uint32_t n)
{
    while (n--) NOP;
}

//PIN POSITIONS ON DISPLAY
#define d0 0
#define d1 1
#define d2 2
#define d3 3
#define d4 4
#define d5 5
#define d6 6
#define d7 7
#define c0 8
#define c1 9 
#define c2 10
#define c3 11
#define c4 12
#define c5 13
#define c6 14
#define c7 15
#define b0 16
#define b1 17
#define b2 18
#define b3 19
#define b4 20
#define b5 21
#define b6 22
#define b7 23
const uint8_t anodes[2*N]   PROGMEM = {c4,c1,d4,c0,b3,d5,b0,b1,b2,d7,b4,d6,c5,b5,c2,c3};
const uint8_t cathodes[2*N] PROGMEM = {b2,c2,c3,b5,c5,b4,d7,d6,c4,b0,b1,d5,b3,d4,c1,c0};

#define READ(x) ((uint8_t)(pgm_read_byte_near(&x)))

int main()
{   
    // ENABLE SERIAL
    UCSR0B = 0x18;
    UCSR0C = 0x86;
    UBRR0L = 0x33;

    uint8_t r,c;
    uint8_t disp[16];
    
    while(1)
    for (r=0;r<16;r++)
    for (c=0;c<8;c++) {
        // SCAN DISPLAY
        uint32_t C = ONE<<READ(cathodes[r]);
        uint32_t A = ONE<<READ(anodes[r>=8?c+8:c]);
        setDDR(0);
        setPort(A);
        if ((disp[r]>>c)&1) setDDR(C|A);
        delay(50);
        
        //HANDLE NEW DATA IF PRESENT
        if (DATA_AVAILABLE)
        {
            uint8_t x = UDR0;
            once(OK_TO_SEND) 
                UDR0 = disp[15]; // SDO
            uint8_t i;
            for (i=15;i>=1;i--)
                disp[i]=disp[i-1];
            disp[0]=x;
        }
    }
} 






