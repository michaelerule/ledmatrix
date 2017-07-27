/*
Hardware notes
===============================================================================

PB1 and PB2 are now used for software serial!

	DDRx   : 1 = output, 0 = input
	PORTx : output buffer
	PINx  : digital input buffer ( writes set pullups )
	Port B is the chip IO port ( other ports are control registers )

		AtTiny24 pinout:
		            __ __
		3     VCC -|  U  |- GND    1
		  BCK PB0 -|     |- PA0 G1
		  SHU PB1 -|     |- PA1 B1
		4 RST PB3 -|     |- PA2 R1
		  PLY PB2 -|     |- PA3 G2 
		  FWD PA7 -|     |- PA4 B2 5
		2 IND PA6 -|_____|- PA5 R2 6
		        
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

=============================================================================*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define NOP __asm__("nop\n\t")
#define N 10
#define NN ((N)*(N))
#define RNGMUL 13
#define RNGMOD 0x4CE3
#define glider  0xCE
#define genesis 0x5E
#define bomb    0x5D
#define TIMEOUT 13
#define SCANRATE 128
//#include "alphabet.h"

uint16_t rng = 79;
uint16_t b0[N+1],b1[N+1];
uint16_t *disp = &b0[0];
uint16_t *buff = &b1[0];
volatile uint8_t scanI = 0;
uint8_t showcased = 0;

uint16_t randInt() {
	return (rng=(RNGMUL*rng+RNGMUL)%RNGMOD);
}

//display scanning interrupt
//PB1 and PB2 are now used for software serial!
ISR(TIM0_COMPA_vect) {
    uint16_t ddr = disp[scanI];
    uint16_t pin = ~(1<<scanI);
    pin &= ddr;
    if (scanI==N) pin=~pin;
    DDRA  = 0  ; DDRB  = 0;
    PORTA = pin; PORTB = pin>>8&0b11;
    DDRA  = ddr; DDRB  = ddr>>8&0b11;
    if (++scanI>N) scanI=0;
}

uint8_t get(uint8_t r, uint8_t c, uint16_t *d)  { 
	if (c<4)	  c += 6;
	else if (c<9) c -= 4;
	else {
		c -= c<13? 3 : 13;
		r += 5;
	}
	if (r==c) c=5;
    return d[r]>>c&1;
}

void set(uint8_t r, uint8_t c, uint8_t v, uint16_t *d)  { 
	if (c<4)	  c += 6;
	else if (c<9) c -= 4;
	else {
		c -= c<13? 3 : 13;
		r += 5;
	}
	if (r==c) c=5;
    if ((d[r]>>c&1)!=v) d[r]^=1<<c;
}

uint8_t prev_c(uint8_t x) { 
	if (x==0) return 17;
	return x-1;
}
uint8_t next_c(uint8_t x) {
	if (x==17) return 0;
	return x+1;
}
uint8_t prev_r(uint8_t x) { 
	if (x==0) return 4;
	return x-1;
}
uint8_t next_r(uint8_t x) {
	if (x==4) return 0;
	return x+1;
}

uint8_t colCount(r,c) {
	return get(prev_r(r),c,disp)+get(r,c,disp)+get(next_r(r),c,disp);
}

void delay(uint32_t n) {
	while (n--) NOP;
}

void flipBuffers() {
	uint16_t *temp = buff; buff = disp; disp = temp;
}

void setup_display() {
	uint8_t i;
	for (i=0; i<=N; i++) buff[i] = disp[i] = 1<<i;
	TIMSK0 = 2;          // Timer CompA interupt
	TCCR0B = 2;          // speed
	TCCR0A = 2;          // CTC mode
	OCR0A  = SCANRATE;   // period
	sei();
}

void one(uint8_t r, uint8_t c, uint16_t *d)  
{
	if (c<4)	  c += 6;
	else if (c<9) c -= 4;
	else {
		c -= c<13? 3 : 13;
		r += 5;
	}
	if (r==c) c=5;
    d[r]|=1<<c;
}

int main() {	
	uint8_t i,j,r,c;
	
	setup_display();
	
	//buff[0] |= 1<<6;
	/*
	for (i=0;i<5;i++) 
		for (j=0;j<2;j++)
			if ((i&1)==(j&1))
			{
				one(i,j,buff);
			}
	flipBuffers();
	while(1){
		for (i=0;i<5;i++) 
			for (j=0;j<18;j++)
			{
				set(i,j,get(i,next_c(j),disp),buff);
				for (c=0;c<255;c++) NOP;
			}
			flipBuffers();
	}
	while(1);
	*/
	
	while(1)
	{
		int input = (PINB&0b00000100)>>2;
		set(r,c,input,disp);
		c ++;
		if (c>17)
		{
			c = 0;
			r++;
			if (r>4) r=0;
		}
	}
	
	while(1)
	for (i=0;i<5;i++) 
	for (j=0;j<18;j++)
	{
		set(i,j,1,disp);
		for (r=0;r<255;r++)
		for (c=0;c<255;c++) NOP;
		set(i,j,0,disp);
	}
	
	while (1) {
		uint8_t alive = 1;
		while (alive) {
			uint8_t k=0;
			uint8_t changed=0;
			for (r=0; r<5; r++) {
				uint8_t previous = colCount(r,17);
				uint8_t current  = colCount(r,0);
				uint8_t neighborsum = previous+current;
				for (c=0; c<18; c++) {
					uint8_t s = get(r,c,disp);
					uint8_t upcoming = colCount(r,next_c(c));
					neighborsum += upcoming;
					uint8_t n = s? (neighborsum+1>>1)==2:neighborsum==3;
					neighborsum -= previous;
					previous = current ;
					current  = upcoming;
					changed |= n!=s && n!=get(r,c,buff);
					k += n;
					set(r,c,n,buff);
				}
			}
			uint8_t dropLife=0;
			if (!(randInt()&7) && !(randInt()&7))      dropLife=genesis;//glider;
			if (!k)                              dropLife=genesis;
			if (!changed && showcased++>TIMEOUT) dropLife=bomb;
			if (dropLife) {
				uint8_t r = randInt()%5;
				uint8_t q = randInt()%18;
				uint8_t a = randInt()&1;
				uint8_t b = randInt()&1;
				uint8_t i,j;
				for (i=0;i<3;i++) {
					uint8_t c = q;
					for (j=0;j<3;j++) {
						set(r,c,(dropLife&0x80)>>7,buff);
						dropLife <<= 1;
						c = a?next_c(c):prev_c(c);
					}
					r = b?next_r(r):prev_r(r);
				}	
				showcased = 0;
			}
			flipBuffers();
		}
	}
} 



