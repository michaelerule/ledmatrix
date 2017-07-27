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
#define NN (N*N)

// we use 11 pins to drive the display
// the IO states are cached here
// we use bit fields to avoid some shifting and masking later
typedef struct {
   unsigned ddr : 11;
   unsigned pin : 11;
} d_state;

uint16_t rng = 79;
d_state dispState[N+1];
volatile uint8_t scanI = 0;

inline uint16_t random() {
	return rng = (31*rng+13)%0x4CE3;
}

inline void flipLight(uint8_t r,uint8_t c) {
	dispState[r==c?N:c].ddr ^= 1<<r;
}

ISR(TIM0_OVF_vect) {
	d_state d = dispState[scanI];
	DDRA  = 0    ; DDRB  = 0;
	PORTA = d.pin; PORTB = d.pin>>8;
	DDRA  = d.ddr; DDRB  = d.ddr>>8;
	if (++scanI>N) scanI=0;
}

int main()
{	
	int i;
	
	for (i=0; i<=N; i++) {
		uint16_t mask = 1<<i;
		dispState[i].ddr =  mask;
		dispState[i].pin = ~mask;
	}
	
	for (i=0; i<200; i++) {
		uint8_t r = random()%N;
		uint8_t c = random()%N;
		flipLight(r,c);
	}

	TCCR0B |= 1<<CS00;
	TIMSK0 |= 1<<TOIE0;
	sei();
	
	while (1) ;
} 



