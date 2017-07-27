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
1350

avrdude -c avrispmkII -p m328p -B1000 -P /dev/ttyUSB0 -F


1 70
2 40
3 30
4 30
5 20
6 20
7 10
8 15
9 14
10 11
11 10
12 9
13 8
14 8
16 7
20 5
30 4
40 3
50 2

=============================================================================*/

#include <avr/io.h>
#include <avr/interrupt.h>

#define NOP __asm__("nop\n\t")
#define N 10
#define NN ((N)*(N))
#define glider  0xCE
#define genesis 0x5E
#define bomb    0x5D
#define TIMEOUT 7
#define SCANINTERVAL 50 
#define PWMMAX 2
#define lightsOff (DDRA=DDRB=0)
#define NBITSTART 1
#define NBITSNEXT(n) (n++)
#include "alphabet.h"

uint32_t rng = 6719;

#define BUFFLEN 32

uint8_t b0[BUFFLEN];
uint8_t b1[BUFFLEN];
uint8_t *buff=&b0[0];
uint8_t *disp=&b1[0];
uint8_t shownFor;

volatile uint8_t portc_extra=0;
volatile uint8_t ddrc_extra=0;
volatile uint16_t adc_data;

uint8_t ll0[NN];
uint8_t ll1[NN];
volatile uint8_t *lightList = &ll0[0];
volatile uint8_t *lightBuff = &ll1[0];
volatile uint8_t lighted = 0;

uint8_t pin_numbers[11] = {0, 1, 2, 6, 7, 10, 11, 12, 13, 14, 15};

uint32_t rintn()
{
    rng = ((uint64_t)rng * 279470273UL) % 4294967291UL;
    return rng%N;
}

uint8_t get(uint8_t *b,uint16_t i)           
{
    return (b[i>>3]>>(i&7))&1;
}

uint8_t set(uint8_t *b,uint16_t i,uint8_t v) {
    if (get(b,i)!=v) 
        b[i>>3]^=1<<(i&7); 
}

#define getLifeRaster(i)    get(disp,i)
#define setLifeRaster(i,v)  set(buff,i,(v))
#define rc2i(r,c)           ((r)*N+(c))
#define getLife(r,c)        getLifeRaster(rc2i(r,c))
#define setLife(r,c,v)      setLifeRaster(rc2i(r,c),v)

uint8_t prev(uint8_t x) {
    return (x>0?x:N)-1;
}

uint8_t next(uint8_t x) {
    return x<N-1?x+1:0;
}

uint8_t columnCount(r,c) {
    return getLife(prev(r),c)
         + getLife(r      ,c)
         + getLife(next(r),c);
}

#define SKIPLIGHTS 8
#define DIAG_OVERSCAN 1

volatile uint8_t scanR = 0;
volatile uint8_t scanC = 0;
volatile uint8_t scanI = 0;
volatile uint8_t scanJ = 40;

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

void flipBuffers() 
{
    uint8_t *temp = buff;
    buff = disp;
    disp = temp;
    
    uint16_t r,c,i=0;
    uint8_t lightCount = 0;
    
    
    for (r=0; r<N; r++)
    {
        for (c=0; c<N; c++)
    	{
		    if (get(disp,i))
		    {
		        lightBuff[lightCount]=(r<<4)|c;
		        lightCount++;
		    }
		    i++;
		}
	}
    volatile uint8_t *ltemp;
    ltemp = lightBuff;
    lightBuff = lightList;
    lightList = ltemp;
    lighted = lightCount;
    OCR0A  = 137/lighted; // period
}

uint32_t getPinstate(uint32_t vector)
{
	uint32_t value = 0;
	uint8_t i;
	for (i=0;i<11;i++) {
		if (vector&1) value |= 1<<pin_numbers[i];
		vector >>= 1;
	}
	return value;
}

ISR(TIMER0_COMPA_vect) 
{
    scanI++;
    scanI%=lighted;
    uint8_t location = lightList[scanI];
    uint8_t r = location>>4;
    uint8_t c = location&(0xf);
    uint32_t xddr,xport;
    if (r==c)
    {
    	xport = getPinstate(1<<N);
    	xddr  = getPinstate(((uint32_t)1<<(c))| (1<<N));
    }
    else
    {
        xport = getPinstate( (~((uint32_t)1<<r)));
        xddr  = getPinstate( (((uint32_t)1<<c)|((uint32_t)1<<r)));
    }
    setDDR(0);
    setPort(xport);
    setDDR(xddr);
}
void delay(uint32_t n)
{
    while (n--) NOP;
}

#define Ll (-8)

int main()
{    
    uint16_t i,r,c,j;
    
    /*
    for (r=0; r<N; r++)
        for (c=0; c<N; c++)
            set(r,c,rintn()&1);
    	flipBuffers();
    */
    	    
    TIMSK0 = 2;        // Timer CompA interupt
    TCCR0B = 2;        // speed
    TCCR0A = 2;        // CTC mode
    OCR0A  = SCANINTERVAL; // period
    sei();
    
    for (r=0; r<N; r++) {
        for (c=0; c<N; c++) {
            set(buff,rc2i(r,c),1);
            set(disp,rc2i(r,c),1);
		flipBuffers();
       }
    }
    
    while(1);
        
    while (1) 
    {
        uint8_t r,c;
        uint8_t changed = 0;
        uint8_t k=0;
        for (r=0; r<N; r++)
        {
            uint8_t previous = columnCount(r,N-1);
            uint8_t current  = columnCount(r,0);
            uint8_t neighbor = previous+current;
            for (c=0; c<N; c++)
            {
                uint8_t cell = getLife(r,c);
                uint8_t upcoming = columnCount(r,next(c));
                neighbor += upcoming;
                uint8_t new = cell? (neighbor+1>>1)==2:neighbor==3;
                neighbor -= previous;
                previous = current ;
                current  = upcoming;
                changed |= new!=cell && new!=get(buff,rc2i(r,c));
                k += new;
                setLife(r,c,new);
            }
        }

        uint8_t l=0;
        if (!rintn()&&!rintn())             l=rintn()&3?genesis:rintn()&1?glider:bomb;
        if (!k)                             l=genesis;
        if (!changed && shownFor++>TIMEOUT) l=rintn()&3?genesis:bomb;
        if (l) {
            uint8_t r = rintn();
            uint8_t q = rintn();
            uint8_t a = rintn()&1;
            uint8_t b = rintn()&1;
            uint8_t i,j;
            for (i=0;i<3;i++)
            {    
                uint8_t c = q;
                for (j=0;j<3;j++) 
                {
                    setLife(r,c,(l&0x80)>>7);
                    l <<= 1;
                    c = a?next(c):prev(c);
                }
                r = b?next(r):prev(r);
            }    
            shownFor = 0;
        }
        flipBuffers();
        delay(100000);
    }
} 

