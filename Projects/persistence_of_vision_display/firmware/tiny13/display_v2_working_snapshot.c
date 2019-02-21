/* Hardware notes
=============================================================================
    DDRx   : 1 = output, 0 = input
    PORTx : output buffer
    PINx  : digital input buffer ( writes set pullups )
    Port B is the chip IO port ( other ports are control registers )

    AtTiny13 pinout:
                    __ __
        4 RST PB5 -|  U  |- VCC       3
              PB3 -|     |- PB2 SCK   5
              PB4 -|     |- PB1 MISO  6
        1     GND -|_____|- PB0 MOSI  2
                
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
        
        
    Programmer pinout, 6 pin, linear:
                
        6 MISO +  
        5 SCK  + 
        4 RST  +  
        3 VCC  +
        2 MOSI +
        1 GND  +
        
    PORT : write to here to set output
    DDR  : write to here to set IO. 1 for output.
    PIN  : pull ups ? digital input ?
    
    
    Circuit:
                    __ __           /-(>)--CA
        GND-www----|  U  |--VCC  /----(<)--CA
               CA--|     |------/   /-(>)--CA
      GND-www-|.|--|     |------------(<)--CA
              GND--|_____|------------(>)--CA
                             \--------(<)--CA
                   
note : not actually the circuit ^. The CPX layout is

    AtTiny13 pinout:
                    __ __
        4 RST PB5 -|  U  |- VCC       3
           *  PB3 -|     |- PB2 *
              PB4 -|     |- PB1 *
        1     GND -|_____|- PB0 *
                

(0)(1)(2)(3)(4)(5)(6)(7)(8)(9)

0 : 1 2
1 : 2 1
2 : 1 0
3 : 0 1
4 : 3 0
5 : 0 3
6 : 2 3
7 : 3 2
8 : 2 0
9 : 0 2

01010100
01010101
11001000
11000100
10010001
10011000
00110010
00110001
01100100
01100010

0101 0100
0101 0101
1100 1000
1100 0100
1001 0001
1001 1000
0011 0010
0011 0001
0110 0100
0110 0010

54
51
C8
c4
91
98
32
31
64
62

uint8_t [] lightcodes = {0x54,0x51,0xC8,0xc4,0x91,0x98,0x32,0x31,0x64,0x62};


0 : 1 2
2 : 1 0
4 : 3 0
8 : 2 0
6 : 2 3
         
         
desired button behavior : 

as soon as it goes from high to low trigger an event
before the next event can trigger, both
    -- wait a certain amount of time
    -- wait for the edge to return to high for a certain amount of time 
 
 attiny13 go fast
 
 -U lfuse:w:0x7a:m
 
 slow
 
 -U lfuse:w:0x6a:m
 
 
avrdude -c avrispmkII -F -p t13 -B1 -P /dev/ttyUSB0 -U lfuse:w:0x7a:m
===========================================================================*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#define NOP __asm__("nop\n\t")
#define N 7
#include <avr/interrupt.h>

uint32_t phase = 128;

volatile int timer_overflow_count = 0;

uint8_t  lightcodes[] = {0x51,0x54,0xC8,0xc4,0x91,0x98,0x31,0x32,0x62,0x64};

const uint8_t pixel_info[] PROGMEM = {
	0,2,2,0,2,2,0,
	2,3,3,2,3,3,2,
	2,3,3,3,3,3,2,
	0,2,3,3,3,2,0,
	0,0,2,3,2,0,0,
	0,0,0,2,0,0,0,};
	
uint8_t disp[N];

volatile uint16_t timer = 0;
volatile uint8_t fast_timer = 0;
volatile uint32_t period_estimate = 500;
volatile uint16_t button_refactory_counter = 0;
volatile uint8_t  button_refactory_count = 0;
volatile uint8_t  button_latch = 0;
volatile uint8_t  l = 0;
volatile uint8_t  scanI = 0;
volatile uint8_t  flip  = 0;

ISR(TIM0_OVF_vect) {
    if (++fast_timer>80) 
    { 
        timer++; 
        fast_timer=0;
        if (++scanI>=N) scanI = 0;
    }
    uint8_t p = lightcodes[l];
    uint8_t d = p>>4;
    uint16_t phaseshift = 64*period_estimate/512;
    p&=0xf;
    DDRB =0;
    PORTB=0;
    if (!flip && timer > period_estimate/2)
    {
        flip = 1;
        scanI = 0;
    }
    if (timer > phaseshift)
    {
        uint8_t state = ((flip? disp[N-1-scanI] : disp[scanI])>>l)&1;
        if (state)
        {
            PORTB=p|0x10;
            DDRB =d;
        }
    } 
    else scanI=5;
    
    l++;
    if (l>7) l=0;
    if (button_refactory_count)
        button_refactory_counter++;
}

int main() { 
 
    uint8_t i,j;
 
    for (j=0;j<N;j++) 
        disp[j]=(int16_t)0;
    for (i=0;i<7;i++) 
        for (j=0;j<N;j++) 
            if (pgm_read_byte_near(&pixel_info[j*7+i])==2)
                disp[j]|=((uint16_t)1)<<i;
            
    TCCR0B |= (1<<CS00);
    TIMSK0 |= 1<<TOIE0;
    DDRB = 0;
    sei();
    while (1)
    {
        if (PINB&0x10) // tilt switch is *open*
        {
            // normally ignore this but two conditions : 
            // -- if the switch is latched start counting
            // -- if we count high enough, release the latch
            // -- make sure this count resets if the switch closes again
            button_refactory_count=1;
            if (button_refactory_counter>800)
            {
                button_latch = 0;
                button_refactory_count=0;
            }
        }
        else // the switch is *closed*
        {
            // if the switch just closed after a suitable time being on, 
            // immediately react
            if (!button_latch)  
            {
                period_estimate = (period_estimate + timer)/2;
                timer = fast_timer = 0;
                flip  = 0;
            }
            // the button is down, but is latched
            // this may be because we already reacted to this edge
            // or the last open period was too short
            // either way we need to start counting again
            button_refactory_count = 0;
            button_refactory_counter = 0;
            button_latch = 1;
        }
    }
} 



