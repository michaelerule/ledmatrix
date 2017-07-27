/*
Hardware notes
===============================================================================

	DDRx   : 1 = output, 0 = input
	PORTx : output buffer
	PINx  : digital input buffer ( writes set pullups )


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

avrdude -c avrispmkII -p m328p -P /dev/ttyUSB0 -U lfuse:w:0xE2:m -U hfuse:w:0xD9:m -U efuse:w:0xFF:m

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
#define SCANRATE 60
#define lightsOff (DDRA=DDRB=0)
#define NBITSTART 1
#define NBITSNEXT(n) (n++)

uint32_t rng = 6719;
uint32_t rintn()
{
    rng = ((uint64_t)rng * 279470273UL) % 4294967291UL;
    return rng &0xf;
}

uint8_t b0[32];
uint8_t b1[32];
uint8_t *buff=&b0[0];
uint8_t *disp=&b1[0];
uint8_t shownFor;

uint8_t ll0[NN];
uint8_t ll1[NN];
volatile uint8_t *lightList = &ll0[0];
volatile uint8_t *lightBuff = &ll1[0];
volatile uint8_t lighted = 0;

uint8_t get(uint8_t *b,uint16_t i)           
{
	return (b[i>>3]>>(i&7))&1;
}

uint8_t set(uint8_t *b,uint16_t i,uint8_t v) {
	if (get(b,i)!=v) 
		b[i>>3]^=1<<(i&7); 
}

//#define getLifeRaster(i)    get(disp,i)
//#define setLifeRaster(i,v)  set(buff,i,(v))
//#define rc2i(r,c)           ((r)*N+(c))
//#define getLife(r,c)        getLifeRaster(rc2i(r,c))
//#define setLife(r,c,v)      setLifeRaster(rc2i(r,c),v)

uint8_t prev(uint8_t x) {
	return (x>0?x:N)-1;
}

uint8_t next(uint8_t x) {
	return x<N-1?x+1:0;
}

/*
uint8_t columnCount(r,c) {
	return getLife(prev(r),c)
         + getLife(r      ,c)
         + getLife(next(r),c);
}
*/
#define SKIPLIGHTS 8

volatile uint8_t scanR = 0;
volatile uint8_t scanC = 0;
volatile uint8_t scanI = 0;

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

ISR(TIMER0_COMPA_vect) 
{
	/*
	do scanC++; while (scanR==scanC);
	if (scanC>=N) 
	{
		scanC=0;
		scanR++;
		if (scanR>N)
		{
			scanR=0;
		}
		uint32_t pins = (uint32_t)1<<scanR;
		if (scanR>N) pins^=-1;
		setPort(pins);
	}
	setDDR( ((uint32_t)1<<scanC)|((uint32_t)1<<scanR) );
	*/
	scanI++;
	scanI%=lighted;
	uint8_t location = lightList[scanI];
	uint8_t r = location>>4;
	uint8_t c = location&0xf;
	setDDR(0);
	if (r==c)
	{
		setPort(((uint32_t)1<<N));
		setDDR(((uint32_t)1<<c)|((uint32_t)1<<N));
	}
	else
	{
		setPort((uint32_t)1<<r);
		setDDR(((uint32_t)1<<c)|((uint32_t)1<<r));
	}
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

void delay(uint8_t n)
{
	while (n--) NOP;
}

int main()
{	
	uint8_t i,r,c;

	/*
	for (i=0; i<100; i++)
		set(buff,rintn()<<4|rintn(),1);
	for (i=0; i<100; i++)
		set(disp,rintn()<<4|rintn(),1);
	*/
	
    TIMSK0 = 2;        // Timer CompA interupt
    TCCR0B = 2;        // speed
    TCCR0A = 2;        // CTC mode
    OCR0A  = SCANRATE; // period
	sei();
	
	for (i=0; i<NN; i++)
	{
		lightList[i] = ((rintn()&0xf)<<4)|(rintn()&0xf);
		uint8_t foo = ((rintn()&0xf)<<4)|(rintn()&0xf);
	}	
	lighted = 10;
	
	
	//for (i=0; i<NN; i++)
	//	set(buff,i,1);
	for (i=0; i<200; i++)
		set(buff,i,1);
	flipBuffers();
	
	/*
	for (i=0; i<=N; i++) 
		buff[i] = disp[i] = 1<<i;

	for (i=0; i<100; i++)
		set(buff,i,1);
	*/
	
	while(1);

	/*
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
	*/
} 



