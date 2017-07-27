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

avrdude -c avrispmkII -p m328p -B100 -P /dev/ttyUSB0 -U lfuse:w:0xE2:m -U hfuse:w:0xD9:m -U efuse:w:0xFF:m -F
avrdude -c avrispmkII -p m328p -B100 -P /dev/ttyUSB0 -U lfuse:w:0xE2:m 
avrdude -c avrispmkII -p m328p -B1000 -P /dev/ttyUSB0 -F

*/

#include <avr/io.h>
#include <avr/interrupt.h>
#define N 8
#define NOP __asm__("nop\n\t")
#include <avr/pgmspace.h>
#define once(condition) {while(!(condition));}

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

/*
c4,c1,d4,c0,b3,d5,b0,b1,b2,d7,b4,d6,c5,b5,c2,c3
b2,c2,c3,b5,c5,b4,d7,d6,c4,b0,b1,d5,b3,d4,c1,c0
*/

const uint8_t anodes[2*N]   PROGMEM = {12,c1,d4,c0,b3,d5,b0,b1,b2,d7,b4,d6,c5,b5,c2,c3};
const uint8_t cathodes[2*N] PROGMEM = {b2,c2,c3,b5,c5,b4,d7,d6,c4,b0,b1,d5,b3,d4,c1,c0};

volatile uint8_t  rowscan = 0;

volatile uint32_t graphics_state = 0;
volatile uint32_t graphics_mask = 0;
volatile uint32_t metatick=0;

uint32_t bb0[16],bb1[16];
uint32_t *disp = &bb0[0];
uint32_t *buff = &bb1[0];

void delay(uint32_t n)
{
    while (n--) NOP;
}

uint64_t rng = 6719;
uint32_t rintn()
{
    rng = ((uint64_t)rng * 279470273UL) % 4294967291UL;
    return (uint32_t)rng;
}

uint8_t get(uint32_t *d,uint8_t r, uint8_t c)  {
    if (r<8) {
        uint8_t temp = r; r = c; c = temp;
    } else {
        uint8_t temp = r; r = c+8; c = temp-8;
    }
    return 1&( d[r] >> ( r<8? cathodes[c] : cathodes[c+8] ) );
}

void flip(uint32_t *d,uint8_t r,uint8_t c)  {
    if (r<8) {
        uint8_t temp = r; r = c; c = temp;
    } else {
        uint8_t temp = r; r = c+8; c = temp-8;
    }
    d[r] ^= ((uint32_t)1)<< ( r<8? cathodes[c] : cathodes[c+8] );
}

void set(uint32_t *d,uint8_t r,uint8_t c,uint8_t v)  {
    if (get(d,r,c)!=v) flip(d,r,c);
}

#include "alphabet.c"

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

volatile uint16_t wait=0;

ISR(TIMER0_COMPA_vect) 
{
    if (wait>0) wait --;
    metatick++;
    if (metatick==30) metatick=0;
    if (!metatick) {
        graphics_state = ((uint32_t)1)<<anodes[rowscan];
        graphics_mask = disp[rowscan] | graphics_state;
        rowscan=(rowscan+1)&15;
    }
    setDDR(0);
    setPort(graphics_state);
    setDDR(graphics_mask);
}

int main()
{    
    uint32_t i,j;
    int32_t ii = 0;
    TIMSK0 = 2; // Timer CompA interupt 1
    TCCR0B = 2; // speed
    TCCR0A = 2; // CTC mode
    OCR0A  = 36;// period
    
    
    /*
    while(1) {
        uint8_t r;
        for (r=0;r<24;r++) {
            setPort(((uint32_t)1)<<r);
            //delay(100000);
        }
        for (r=0;r<24;r++) {
            setPort(~(((uint32_t)1)<<r));
            //delay(200000);
        }
    }
    */
        
    DDRB=DDRC=DDRD=0xff;    
    while(1) {
        /*
        c4,c1,d4,c0,b3,d5,b0,b1,b2,d7,b4,d6,c5,b5,c2,c3
        b2,c2,c3,b5,c5,b4,d7,d6,c4,b0,b1,d5,b3,d4,c1,c0
        */
        uint8_t r;
        for (r=0;r<16;r++) {
            setPort(((uint32_t)1)<<(uint8_t)(pgm_read_byte_near(&anodes[r])));
            delay(100);
            setPort(0);
        }
        for (r=0;r<16;r++) {
            setPort(~(((uint32_t)1)<<(uint8_t)(pgm_read_byte_near(&cathodes[r]))));
            delay(100);
            setPort(0);
        }
    }
   
    
    sei();
    while(1);
} 

int main()
{	
    UBRRL = 0x1A;                // Set the baud rate 
    UCSRB = 1<<RXEN  | 1<<TXEN ; // Enable UART receive transmit 
    UCSRC = 1<<UCSZ1 | 1<<UCSZ0; // 8 data bits, 1 stop bit 
    
    uint32_t display_memory[(N+1)*2];
    uint32_t *disp = &display_memory[0];
    uint32_t *buff = &display_memory[N+1];
    uint32_t *temp;
    uint32_t pin=0, ddr=0, rx_n=0, mask=0;
    uint8_t  rx_i=0, d=0, i=0, j=0, k=0, x=0, w_i=0, r_int=0;
    
    for (i=0;i<2*(N+1);i++) display_memory[i]=0xfffffff;
    
    while(1) 
    for (d=0   ; d<3 ; d++) 
    for (k=1<<d; k>0 ; k--)
    for (i=0   ; i<=N; i++)
    {
        pin = 1<<i;
        ddr = disp[i];
        ddr = (i!=N)? ((ddr>>9*d)&0x1ff) : ((ddr>>10*d)&0x3ff);
        mask = (1<<i)-1;
        ddr = (ddr&mask)|((ddr&~mask)<<1);
        if (i>=5) pin<<=5;
        ddr = 0x1F&ddr|0xfc00&(ddr<<5)|pin;
        if (i!=N) pin=~pin;
        pin &= ddr;
        DDRA = DDRB = DDRD = 0;
        PORTB = pin;     DDRB = ddr;
        PORTD = pin>>8;  DDRD = ddr>>8;
        PORTA = pin>>15; DDRA = ddr>>15;
        if (DATA_AVAILABLE)
        {
            x = UDR;
            once(OK_TO_SEND) UDR = x;
            if (x&0x80)
            {
                r_int = rx_i = 1;
                rx_n = x&0x7f;
            }
            else if (r_int) {
                rx_n <<=7;
                rx_n |= x&0x7f;
                if (rx_i==3)
                {
                    if (rx_n>>30==1) w_i=0;
                    buff[w_i]=rx_n&0x3FFFFFFF;
                    if (++w_i>N) {
                        w_i=0;
                        temp = disp; 
                        disp = buff; 
                        buff = temp;
                    }
                    r_int = 0;
                }
                rx_i++;
            }
        }
    }
} 






