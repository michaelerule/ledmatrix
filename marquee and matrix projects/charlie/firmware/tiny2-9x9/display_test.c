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

April Karen Emma Nina

=============================================================================*/

#include <avr/io.h>
#define NOP __asm__("nop\n\t")
#include <avr/interrupt.h>

#define N 9
#define NN ((N)*(N))
#define RNGMUL 13
#define RNGMOD 0x4CE3
#define glider  0xCE
#define genesis 0x5E
#define bomb    0x5D
#define TIMEOUT 13
#define SCANRATE 128

uint16_t rng = 79;
uint16_t b0[N+1],b1[N+1];
uint16_t *disp = &b0[0];
uint16_t *buff = &b1[0];
volatile uint8_t scanI = 0;
uint8_t showcased = 0;

/*
uint16_t randInt() {
	return (rng=(RNGMUL*rng+RNGMUL)%RNGMOD);
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
	if (x==0) return 7;
	return x-1;
}
uint8_t next_c(uint8_t x) {
	if (x==8) return 0;
	return x+1;
}
uint8_t prev_r(uint8_t x) { 
	if (x==0) return 7;
	return x-1;
}
uint8_t next_r(uint8_t x) {
	if (x==8) return 0;
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

    0 1 2 3 4 5 6 7 8
    8 7 6 5 0 1 2 3 4
0 4 
1 3
2 2 
3 1 
4 0
5 6 
6 7 
7 8 
8 9 

[5]


*/

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
	if (c<4) c = 8-c;
	else     c -= 4;
	if (r<5) r = 4-r;
	else     r += 1;
	if (r==c) c=5;
    d[r]|=1<<c;
}

int main() {	
	uint8_t i,j,r,c;
	
	
	while(1)
	{
		for (c=0;c<11;c++)
			for (r=0;r<11;r++) 
			{
				uint16_t pin = ((uint16_t)1)<<c;
				uint16_t ddr = pin | (((uint16_t)1)<<r);
				DDRA  = 0  ; DDRB  = 0;
				PORTA = pin; PORTB = pin>>8&0b111;
				DDRA  = ddr; DDRB  = ddr>>8&0b111;

				for (i=0;i<56;i++)  NOP;
			}
	}
	
	
	setup_display();
	
	for (c=0;c<11;c++) for (r=0;r<11;r++) 
	{
		one(r,c,disp);
		for (i=0;i<255;i++) NOP;
		for (i=0;i<255;i++) NOP;
		for (i=0;i<255;i++) NOP;
	}
	while(1);
} 



