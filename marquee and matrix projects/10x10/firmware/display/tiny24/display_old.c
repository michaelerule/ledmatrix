/*
Hardware notes
==============

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
    
*/


// STANDARD DEFINITIONS
#define HIGH   1
#define LOW    0
#define INPUT  0
#define OUTPUT 1

#define NOP __asm__("nop\n\t")

#define DIGITAL_OUT_PORT PORTA
#define DIGITAL_IN_PORT  PINA
#define DIGITAL_DIRECTION_PORT DDRA
#define inp(port) (port)
#define outp(val, port) (port)=(val)
#define inb(port) (port)
#define outb(port, val) (port)=(val)
#define sbi(port, bit) (port)|=(1<<(bit))
#define cbi(port, bit) (port)&=~(1<<(bit)) 
#define PINMODES(MODES) (DIGITAL_DIRECTION_PORT=(MODES))
#define PIN_MODE(PIN,MODE) (PINMODES(DIGITAL_DIRECTION_PORT&~(1<<(PIN))|((MODE)<<(PIN))))
#define DIGITAL_WRITE_HI(PINS) (DIGITAL_OUT_PORT|=(PINS))
#define DIGITAL_WRITEXOR(PINS) (DIGITAL_IN_PORT=(PINS))
#define DIGITAL_WRITEALL(PINS) (DIGITAL_OUT_PORT=(PINS))
#define DIGITAL_WRITELOW(PINS) (DIGITAL_OUT_PORT&=~(PINS))
#define DIGITAL_OUT_STAT()     (DIGITAL_OUT_PORT)
#define DIGITAL_WRITE(PIN,VAL) (DIGITAL_OUT_PORT=(DIGITAL_OUT_PORT&~(1<<(PIN)))|((VAL)<<(PIN)))
#define DIGITAL_READALL() (DIGITAL_IN_PORT)
#define DIGITAL_READ(PIN) ((DIGITAL_IN_PORT>>(PIN))&1)
#define ever (;;)
#define pinMode PIN_MODE
#define digitalWrite DIGITAL_WRITE

#define DELAYMS 27
#include <avr/pgmspace.h>
#include <avr/io.h>

#define N 10
#define NN ((N)*(N))


//we have a "high quality" 8-bit random number generator
uint16_t random_state    = 79;
inline uint16_t random()
{
    random_state = (31*random_state+13)%19683;
    return random_state;
}

uint8_t lifeState[NN*2/8+1]; //careful : ensure this is not off by 1

//limited mostly by stack space
#define MAXLIGHTSON 40
uint8_t onLights[MAXLIGHTSON];

uint8_t numberOn;
uint8_t oldNumberOn;

uint8_t lifeFlip = 0;

uint8_t showcased;
uint8_t changed;
uint8_t scantic = 0;

void flipLife() {
	lifeFlip = 1-lifeFlip;
}

#define getLifeIndex(r,c) ((((r)*N+(c))<<1)+lifeFlip)
#define getLifeBitIndex(i) ((i)&7)
#define getLifeByteIndex(i) ((i)>>3)

inline uint8_t getLife(uint8_t r,uint8_t c) {
	uint8_t i = getLifeIndex(r,c);
	return (lifeState[getLifeByteIndex(i)]>>getLifeBitIndex(i))&1;
}
inline void setLife(uint8_t r,uint8_t c,uint8_t v) {
	uint8_t i = getLifeIndex(r,c);
	uint8_t state = lifeState[getLifeByteIndex(i)];
	lifeState[getLifeByteIndex(i)] = (state&~(1<<getLifeBitIndex(i)))|(v<<getLifeBitIndex(i));
}
inline uint8_t getOldLife(uint8_t r,uint8_t c) {
	uint8_t temp;
	flipLife();
	temp = getLife(r,c);
	flipLife();
	return temp;
}
inline void setNewLife(uint8_t r,uint8_t c,uint8_t v) {
	flipLife();
	setLife(r,c,v);
	flipLife();
}

inline void lightsOff() {
	DDRA = DDRB = 0;
}
inline void lighton(uint8_t r, uint8_t c) {
	uint16_t q=0,Q=0;
	if (r!=c) {
		q = (1<<r)|(1<<c);
		Q = 1<<c;
		DDRA = DDRB = 0;
		PORTA = Q ;
		PORTB = Q >> 8;
		DDRA = q ;
		DDRB = q >> 8;
	} else {
		q = 1<<r;
		DDRA = DDRB = 0;
		DDRA = PORTA = q ;
		PORTB = 0 ;
		DDRB = 4 ;
	}
}
inline void dolight(uint8_t r, uint8_t c) {
	uint16_t t=0,u=0;
	lighton(c,r);
}
inline void delay(uint8_t k) {
	for (;k;k--) NOP;
}
inline void scanDisplay() {
	uint8_t  r,c;
    uint8_t loc = onLights[scantic];
    if (oldNumberOn) {
		r =  loc     & 0xf;
		c = (loc>>4) & 0xf;
		if (r!=7 || c!=3) dolight(r,c);
		else dolight(c,r);
		//delay(10);
		lightsOff();
		scantic = (scantic + 1)%oldNumberOn;
	}
}

int main()
{	
	uint8_t  r,c,x,i,j,y,*temp;
	
	//pseudorandom initialization
	for (r=0; r<N; r++)
		for (c=0; c<N; c++) 
			setLife(r,c,!!(random()&3));
	
	for ever {
     	changed  = 0;
		numberOn = 0;
		for (r=0;r<N;r++) for (c=0;c<N;c++) {
			uint8_t n=0, newstate=0;
			uint8_t I=0,J=0;
			scanDisplay();
			
			y = (r+N-1)%N;
			x = (c+N-1)%N;
			i = (r+1)%N;
			j = (c+1)%N;
			n += getLife(y,x)&1;
			n += getLife(y,c)&1;
			n += getLife(y,j)&1;
			n += getLife(r,x)&1;
			n += getLife(r,j)&1;
			n += getLife(i,x)&1;
			n += getLife(i,c)&1;
			n += getLife(i,j)&1;

			newstate = getLife(r,c)? !(n<2||n>3):(n==3);
			scanDisplay();
			if (getLife(r,c)!= newstate && newstate != getOldLife(r,c)) 
				changed=1;
			scanDisplay();
			//changed = 1;
			
			if (newstate) {
			scanDisplay();
				if (numberOn<MAXLIGHTSON)
					onLights[numberOn++] = r|(c<<4);
				else
					onLights[random()%MAXLIGHTSON] = r|(c<<4);
			scanDisplay();
			}
			setNewLife(r,c,newstate);
			scanDisplay();
		}
		
			scanDisplay();
		flipLife();
			scanDisplay();
		oldNumberOn  = numberOn;
			scanDisplay();
		scantic      = 0;
		
			scanDisplay();
		// add things so it doesn't get dull
		if (!numberOn || !changed && showcased>10) {
			r = random()%N;
			c = random()%N;
			x = ((c+N)-1)%N;
			y = ((r+N)-1)%N;
			i = (r+1)%N;
			j = (c+1)%N;
			setLife(r,c,1);
			setLife(r,x,1);
			setLife(r,j,1);
			setLife(i,c,1);
			setLife(y,x,1);
			showcased = 0;
		}
		if (!changed) showcased++;
			scanDisplay();
		
	}
	//control flow should not reach here
} 



