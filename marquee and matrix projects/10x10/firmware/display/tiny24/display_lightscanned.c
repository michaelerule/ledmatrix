/*
Hardware notes
===============================================================================

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

#define NOP __asm__("nop\n\t")
#define N 10
#define NN ((N)*(N))
#define RNGMUL 13
#define RNGMOD 0x4CE3
#define glider  0xCE
#define genesis 0x5E
#define bomb    0x5D
#define TIMEOUT 7
#define SCANRATE 150
#define lightsOff (DDRA=DDRB=0)
#include "alphabet.h"

uint16_t rng = 79;
uint16_t b0[N+1],b1[N+1];
uint16_t *disp = &b0[0];
uint16_t *buff = &b1[0];
volatile uint8_t nLighted = 0;
volatile uint8_t scanR = 0;
volatile uint8_t scanC = 0;
volatile uint8_t scanI = 0;
uint8_t showcased = 0;

uint16_t randIntN() {
	return (rng=(RNGMUL*rng+RNGMUL)%RNGMOD)%N;
}

uint8_t get(uint8_t r, uint8_t c,uint16_t *d)  {
	return d[r==c?N:c]>>r&1;
}

void set(uint8_t r,uint8_t c,uint8_t v,uint16_t *d)  {
	if (get(r,c,d)!=v) d[r==c?N:c]^=1<<r;
}

uint8_t prev(uint8_t x) {
	return (x>0?x:N)-1;
}

uint8_t next(uint8_t x) {
	return x<N-1?x+1:0;
}

#define MAXON 20
volatile uint8_t lighted[MAXON];
void cacheDisplayList()
{
	uint8_t r,c,k=0;
	nLighted = 0;
	for (r=0; r<N; r++)
		for (c=0; c<N; c++)
			if (get(r,c,disp)) {
				lighted[k++]=r|(c<<4);
				if (k>MAXON) { nLighted=k; return; }
			}
	nLighted=k;
	return;
}

ISR(TIM0_COMPA_vect) {
	if (!nLighted) {
		DDRA = DDRB = 0;
		return;
	}
	scanI++;
	uint8_t location = lighted[scanI%nLighted];
	uint8_t r = location & 0xf;
	uint8_t c = location >> 4;

	uint16_t ddr = 1<<(r==c?N:c);
	uint16_t pin = ~ddr;
	ddr |= 1<<r;
	PORTA = pin; 
	PORTB = pin>>8;
	DDRA = ddr; 
	DDRB = ddr>>8;


	/*
	uint16_t ddr = 1<<scanC;
	if (get(scanR,scanC<N?scanC:scanR,disp))
	{
		ddr |= 1<<scanR;
	}	
	DDRA  = ddr; DDRB  = ddr>>8;
	do scanR++; while(scanR==scanC);
	if (scanR >= N) {
		scanR = 0; 
		scanC = scanC==N? 0 : scanC+1;
	
		uint16_t pin = ~(1<<scanC);
		PORTA = pin; 
		PORTB = pin>>8;
	}
	*/
}

uint8_t colCount(r,c) {
	return get(prev(r),c,disp)+get(r,c,disp)+get(next(r),c,disp);
}

void flipBuffers() {
	uint16_t *temp = buff;
	buff = disp;
	disp = temp;
	cacheDisplayList();
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
		//ch <<=5;
		while (chw) {
			uint16_t v = ch&0x1f;
			uint8_t r,c;
			for (r=0; r<=N; r++) buff[r] = disp[r];
			for (r=1;r<N;r++)
				for (c=0;c<5;c++)
					set(c+h,r-1,get(c+h,r,disp),buff);
			for (c=0;c<5;c++) {
				set(c+h,N-1,v&1,buff);
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

int main()
{	
	uint8_t i;
	for (i=0; i<=N; i++) buff[i] = disp[i] = 1<<i;
	
    TIMSK0 = 2;          // Timer CompA interupt
    TCCR0B = 1;          // speed
    TCCR0A = 2;          // CTC mode
    OCR0A  = SCANRATE;   // period
	sei();
	
	//for (i=0; i<20; i++)
	//	set(randIntN(),randIntN(),1,disp);

	while(1) {//nLighted=1;
	scrollText(5,&titlestring1[0]);
	scrollText(0,&titlestring2[0]);
	uint32_t r;
	for (r=0; r<65535; r++) NOP;
	}
	/*
	while (1) {

		uint8_t r,c;
		uint8_t k=0;
		uint8_t changed=0;
		for (r=0; r<N; r++)
		{
			uint8_t previous = colCount(r,N-1);
			uint8_t current  = colCount(r,0);
			uint8_t neighborsum = previous+current;
			for (c=0; c<N; c++)
			{
				uint8_t s = get(r,c,disp);
				uint8_t upcoming = colCount(r,next(c));
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
		if (!randIntN() && !randIntN())      dropLife=glider;
		if (!k)                              dropLife=genesis;
		if (!changed && showcased++>TIMEOUT) dropLife=bomb;
		if (dropLife) {
			uint8_t r = randIntN();
			uint8_t q = randIntN();
			uint8_t a = randIntN()&1;
			uint8_t b = randIntN()&1;
			uint8_t i,j;
			for (i=0;i<3;i++)
			{
				uint8_t c = q;
				for (j=0;j<3;j++) 
				{
					set(r,c,(dropLife&0x80)>>7,buff);
					dropLife <<= 1;
					c = a?next(c):prev(c);
				}
				r = b?next(r):prev(r);
			}	
			showcased = 0;
		}
		//nLighted=k;
		flipBuffers();
	}*/
} 



