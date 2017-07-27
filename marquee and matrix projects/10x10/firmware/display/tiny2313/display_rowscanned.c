/*
Hardware notes
===============================================================================

	DDRx   : 1 = output, 0 = input
	PORTx : output buffer
	PINx  : digital input buffer ( writes set pullups )
	Port B is the chip IO port ( other ports are control registers )

		AtTiny2313 pinout:
		           __ __
		 RESET A2-|  U  |-VCC
			RX D0-|     |-B7 SCK
			TX D1-|     |-B6 MISO
			   A1-|     |-B5 MOSI
			   A0-|     |-B4
			   D2-|     |-B3
			   D3-|     |-B2
			   D4-|     |-B1
			   D5-|     |-B0
			  GND-|_____|-D6
		        
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
#define TIMEOUT 13
#define SCANRATE 32

uint16_t rng = 79;
uint16_t b0[N+1],b1[N+1];
uint16_t *disp = &b0[0];
uint16_t *buff = &b1[0];
volatile uint8_t scanI = 0;
volatile uint8_t scanJ = 0;
uint8_t showcased = 0;
uint8_t pins[11] = {0,1,2,3,4,10,11,12,13,14,15};

uint32_t getPinstate(uint32_t vector)
{
	uint32_t value = 0;
	uint8_t i;
	for (i=0;i<11;i++) {
		value |= (vector&1)<<pins[i];
		vector >>= 1;
	}
	return value;
}

void setDDR(uint32_t ddr) 
{
    DDRB = ddr;
    ddr >>= 8;
    DDRD = ddr&0x7f;
    ddr >>= 7;
    DDRA = ddr&3;
}

void setPort(uint32_t pins)
{
    PORTB = pins;
    pins >>= 8;
    PORTD = pins&0x7f;
    pins >>= 7;
    PORTA = pins&3;
}

uint16_t randIntN() {
	return (rng=(RNGMUL*rng+RNGMUL)%RNGMOD)%N;
}

void scanlight() {
	uint32_t pin = 1<<pins[scanI];
	uint32_t ddr=getPinstate(disp[scanI]);
	//uint32_t ddr=((disp[scanI]>>scanJ)&1)<<pins[scanJ];
	ddr |= pin;
	if (scanI!=N) pin=~pin;
	pin &= ddr;
	setDDR(0);
	setPort(pin);
	setDDR(ddr);
	//if (++scanJ>=N) {
	//	if (++scanI>N) scanI=0;
	//	scanJ=0;
	//}
	if (++scanI>N) scanI=0;
}

ISR(TIMER0_COMPA_vect)  {scanlight();}
ISR(TIMER1_COMPA_vect)  {scanlight();}
ISR(TIMER0_OVF_vect)   {scanlight();}
ISR(TIMER1_OVF_vect)   {scanlight();}

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

uint8_t colCount(r,c) {
	return get(prev(r),c,disp)+get(r,c,disp)+get(next(r),c,disp);
}

void flipBuffers() {
	uint16_t *temp = buff;
	buff = disp;
	disp = temp;
}


void timer1(void) 
{ 
   OCR1A = SCANRATE;               //Set OCR1A 
   TCCR1A = 0x00;                //Timer counter control register 
   TCCR1B = (0 << WGM13)|(1 << WGM12)|(0 << CS12)|(1 << CS11)|(1<< CS10); // WGM1=4, prescale at 1024 
   TIMSK |= (1 << OCIE1A);         //Set bit 6 in TIMSK to enable Timer 1 compare interrupt. 
} 

int main()
{	
	uint8_t r,c;
	uint8_t i;
	/*
	for (r=0; r<N; r++)
		for (c=0; c<N; c++)
		{
			set(r,c,1,disp);
		}
	*/
	for (i=0; i<20; i++)
		set(randIntN(),randIntN(),1,disp);
	timer1();		
	sei();
	//while (1);
	while (1) {

	uint8_t alive = 1;
	while (alive) {
		
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
		if (!k)                              {dropLife=genesis;}
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
		flipBuffers();
	}
	}
} 



