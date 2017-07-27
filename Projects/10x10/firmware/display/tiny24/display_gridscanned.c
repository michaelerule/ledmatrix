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
#define SCANRATE 40
#define lightsOff (DDRA=DDRB=0)
#define NBITSTART 1
#define NBITSNEXT(n) (n++)

uint16_t rng = 79;
uint16_t rintn() { return (rng=(RNGMUL*rng+RNGMUL)%RNGMOD)%N; }

uint8_t b0[13];
uint8_t b1[13];
uint8_t *buff=&b0[0];
uint8_t *disp=&b1[0];
uint8_t shownFor;

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
#define rc2i(r,c)           ((r)*10+(c))
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

volatile uint8_t scanR = 0;
volatile uint8_t scanC = 0;
ISR(TIM0_COMPA_vect) {
	lightsOff;
	do scanR++; while (scanR==scanC);
	if (scanR>=N) {
		scanR=0;
		scanC++;
		if (scanC>N) {
			scanC = 0;
		}
		uint16_t pin = ~(1<<scanC);
		PORTA = pin; 
		PORTB = pin>>8;
	}
	if (getLife(scanR,scanC))
	{
		uint16_t ddr = 1<<scanC | 1<<scanR;
		DDRA = ddr; 
		DDRB = ddr>>8;
		return;
	}
}

void flipBuffers() 
{
	uint8_t *temp = buff;
	buff = disp;
	disp = temp;
}


int main()
{	
	uint8_t i;

	for (i=0; i<=N; i++) 
		buff[i] = disp[i] = 1<<i;
	
    TIMSK0 = 2;        // Timer CompA interupt
    TCCR0B = 2;        // speed
    TCCR0A = 2;        // CTC mode
    OCR0A  = SCANRATE; // period
	sei();

	//for (i=0; i<100; i++)
	//	set(buff,i,1);
	
	for (i=0; i<100; i++)
		set(buff,rc2i(rintn(),rintn()),1);
	flipBuffers();


	//while(1);

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
		if (!rintn() && !rintn())           l=glider;
		if (!k)                             l=genesis;
		if (!changed && shownFor++>TIMEOUT) l=bomb;
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



