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

=============================================================================*/

#include <avr/io.h>
#include <avr/interrupt.h>

#define NOP __asm__("nop\n\t")
#define N 16
#define NN ((N)*(N))
#define glider  0xCE
#define genesis 0x5E
#define bomb    0x5D
#define TIMEOUT 7
#define SCANINTERVAL 45
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

uint32_t rintn()
{
    rng = ((uint64_t)rng * 279470273UL) % 4294967291UL;
    //rng ^= adc_data;
    //uint16_t smaller = (rng>>16)^rng;
    //smaller = (smaller>>8)^smaller & 0xff;
    //smaller = (smaller>>4)^smaller & 0xff;
    return rng%N;//smaller;
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
    DDRC = ddr|ddrc_extra;
}

void setPort(uint32_t pins)
{
    PORTB = pins;
    pins >>= 8;
    PORTD = pins;
    pins >>= 8;
    PORTC = pins|portc_extra;
}

void flipBuffers() 
{
    uint8_t *temp = buff;
    buff = disp;
    disp = temp;
    
    uint16_t r,c,i;
    uint8_t lightCount = 0;
    for (i=0;i<NN;i++)
    {
        if (get(disp,i))
        {
            lightBuff[lightCount]=i;
            lightList[lightCount]=i;
            lightCount++;
            lighted=lightCount;
        }
    }
    volatile uint8_t *ltemp;
    ltemp = lightBuff;
    lightBuff = lightList;
    lightList = ltemp;
    lighted = lightCount;
}

const uint8_t pwm_setting[6] = {2,1,2,2,16,0};
const uint8_t pwm_info[256] = {
 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 5,
 0, 0, 2, 3, 2, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 5,
 0, 2, 3, 3, 3, 2, 0, 0, 0, 1, 2, 2, 2, 1, 0, 5,
 2, 3, 3, 3, 3, 3, 2, 0, 1, 2, 2, 2, 2, 2, 1, 5,
 2, 3, 3, 2, 3, 3, 2, 0, 1, 2, 2, 1, 2, 2, 1, 5,
 0, 2, 2, 0, 2, 2, 0, 0, 0, 1, 1, 0, 1, 1, 0, 5,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 5,
 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 1, 3, 1, 0, 0, 5,
 0, 2, 1, 1, 1, 2, 0, 0, 0, 1, 3, 3, 3, 1, 0, 5,
 2, 1, 1, 1, 1, 1, 2, 0, 1, 3, 3, 3, 3, 3, 1, 5,
 2, 1, 1, 2, 1, 1, 2, 0, 1, 3, 3, 1, 3, 3, 1, 5,
 0, 2, 2, 0, 2, 2, 0, 0, 0, 1, 1, 0, 1, 1, 0, 5,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

volatile uint8_t pwm_wait;
volatile uint8_t pwm_on;
volatile uint8_t analog_count = 0;

ISR(TIMER0_COMPA_vect) 
{
	if (pwm_wait)
	{
    	if (pwm_on>1)
    	{
    		pwm_on--;
		} else if (pwm_on==1)
		{
    		setDDR(0);
    		pwm_on--;
		}
		pwm_wait--;
		return;
	}
    scanI++;
    scanI%=lighted;
    uint8_t location = lightList[scanI];
    uint8_t r = location>>4;
    uint8_t c = location&0xf;
    setDDR(0);
    if (r==c)
    {
    	PORTC = 1;
    	DDRC = 1;
    	setPort(1);
    	setDDR(((uint32_t)1<<(c+1))| 1);
        if (scanJ)
        {
            scanJ=DIAG_OVERSCAN;
        }
        else
        {
            scanJ--;
            scanI--;
        }
    }
    else
    {
        setPort( (~((uint32_t)1<<r)) <<1);
        setDDR( (((uint32_t)1<<c)|((uint32_t)1<<r)) <<1);
    }
	pwm_wait=PWMMAX;
	pwm_on=pwm_setting[pwm_info[location]];
	if (pwm_on==16)
	{
		setDDR(0);
		pwm_on=0;
	}
}
void delay(uint32_t n)
{
    while (n--) NOP;
}

#include "pstrings.h"
#define TSLOAD(i,buffer) (pgm_read_byte_near(&buffer[i]))

void scrollText(uint8_t h,uint8_t *buffer)
{
	uint8_t i=0;
	uint8_t ci = TSLOAD(i,buffer);
	while (ci!=255) {
		uint32_t ch = loadChar(ci);
		uint8_t  chw = (ch>>(32-3)&7);
		ch &=0x1fffffff;
		while (chw) {
			uint16_t v = ch&0x1f;
			uint8_t r,c;
			for (r=0; r<=BUFFLEN; r++) buff[r] = disp[r];
			for (r=1;r<N;r++)
				for (c=0;c<5;c++)
					set(buff,(c+h)*N+r-1,get(disp,(c+h)*N+r));
			for (c=0;c<5;c++) {
				set(buff,(c+h)*N+N-1,v&1);
				v>>=1;
			}
			chw--;
			ch>>=5;
			flipBuffers();
		}
		i++;
		ci = TSLOAD(i,buffer);
	}
}

#define Ll (-8)

int main()
{    
    uint16_t i,r,c,j;
    
    TIMSK0 = 2;        // Timer CompA interupt
    TCCR0B = 2;        // speed
    TCCR0A = 2;        // CTC mode
    OCR0A  = SCANINTERVAL; // period
    sei();
    

    set(buff,16*2-5,1);
    set(buff,16*3-6,1);
    set(buff,16*3-4,1);
    set(buff,16*4-7,1);
    set(buff,16*4-3,1);
    set(buff,16*5-8,1);
    set(buff,16*5-2,1);
    set(buff,16*6-8,1);
    set(buff,16*6-2,1);
    set(buff,16*7-7,1);
    set(buff,16*7-3,1);
    set(buff,16*7-6,1);
    set(buff,16*7-4,1);
    set(buff,16*6-5,1);
    flipBuffers();
	delay(65535);
    
    uint8_t K = 8;
    
    flipBuffers();
    set(buff,16*(K+2)-5,1);
    set(buff,16*(K+3)-6,1);
    set(buff,16*(K+3)-4,1);
    set(buff,16*(K+4)-7,1);
    set(buff,16*(K+4)-3,1);
    set(buff,16*(K+5)-8,1);
    set(buff,16*(K+5)-2,1);
    set(buff,16*(K+6)-8,1);
    set(buff,16*(K+6)-2,1);
    set(buff,16*(K+7)-7,1);
    set(buff,16*(K+7)-3,1);
    set(buff,16*(K+7)-6,1);
    set(buff,16*(K+7)-4,1);
    set(buff,16*(K+6)-5,1);
    flipBuffers();
	delay(65535);
    
    flipBuffers();
    set(buff,16*(K+2)-5+Ll,1);
    set(buff,16*(K+3)-6+Ll,1);
    set(buff,16*(K+3)-4+Ll,1);
    set(buff,16*(K+4)-7+Ll,1);
    set(buff,16*(K+4)-3+Ll,1);
    set(buff,16*(K+5)-8+Ll,1);
    set(buff,16*(K+5)-2+Ll,1);
    set(buff,16*(K+6)-8+Ll,1);
    set(buff,16*(K+6)-2+Ll,1);
    set(buff,16*(K+7)-7+Ll,1);
    set(buff,16*(K+7)-3+Ll,1);
    set(buff,16*(K+7)-6+Ll,1);
    set(buff,16*(K+7)-4+Ll,1);
    set(buff,16*(K+6)-5+Ll,1);
    flipBuffers();
	delay(65535);
   
    uint8_t l=genesis;
    r = 7;
    uint8_t q = 8;
    uint8_t a = 0;
    uint8_t b =01;
    flipBuffers();
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
    flipBuffers();
	delay(655350);
    
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
    }
} 

