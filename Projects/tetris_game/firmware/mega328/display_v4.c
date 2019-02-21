/*
Hardware notes
===============================================================================

DDRx   : 1 = output, 0 = input
PORTx : output buffer
PINx  : digital input buffer ( writes set pullups )

         !RESET     PC6 -|  U  |- PC5
                    PD0 -|     |- PC4
                    PD1 -|     |- PC3
                    PD2 -|     |- PC2
                    PD3 -|  m  |- PC1
                    PD4 -|  3  |- PC0
                    VCC -|  2  |- GND
                    GND -|  8  |- AREF
                    PB6 -|     |- AVCC
                    PB7 -|     |- PB5   SCK  ( yello )
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

0 a0
1 c3
2 c5
3 a3
4 c0
5 a1
6 c6
7 c7

8 a2
9 a5
A c4
B a7
C c2
D c1
E x 
F x

G a6
H a4

4 c0
D c1
C c2
1 c3
A c4
2 c5
6 c6
7 c7

0 a0
5 a1
8 a2
3 a3
F a4
9 a5
E a6
B a7

0 a0
5 a1
8 a2
3 a3
H a4
9 a5
G a6
B a7

4 c0
D c1
C c2
1 c3
A c4
2 c5
6 c6
7 c7

E x 
F x


x 00
x 01
o 
x 03

x  01
o  
xx 03 13

xx 01 11
o
x  03

ox 12
xx 03 13

x  0
ox 1
 x 1
 
 x 1
ox 1
x  0

x  0
ox 1
x  0

xx
xx
ox
xx

2 bits row 1 bits column: 9 bits

============================================================================*/

#include <avr/io.h>
#include <avr/interrupt.h>
#define N 8
#define NOP __asm__("nop\n\t")
#include <avr/pgmspace.h>
#include "scale.c"
#include "blues.c"
#include "chromatic_up.c"
#include "chromatic_down.c"

#define speakerpin ((uint32_t)1<<23)

uint8_t the_1[4] = {0x00,0x01,0x02,0x03};
uint8_t the_L[4] = {0x01,0x02,0x03,0x13};
uint8_t the_J[4] = {0x01,0x11,0x02,0x03};
uint8_t the_o[4] = {0x02,0x12,0x03,0x13};
uint8_t the_s[4] = {0x01,0x02,0x12,0x13};
uint8_t the_z[4] = {0x11,0x02,0x12,0x03};
uint8_t the_t[4] = {0x01,0x02,0x12,0x03};
uint8_t* shapes[7] = {the_L,the_1,the_J,the_o,the_s,the_z,the_t};

/*
0?12?76?8?9


0
3
1
2
5
7
6
4

0
3
1
2
5
7
6
4


4
2
7
1
0
3
5
6


*/
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
#define INITIALSPEED 100

uint8_t anodes[2*N]   = {c2,b6,b2,d0,b4,b1,d4,d7, d5,d6,b3,b0,c0,d1,c1,c3};
uint8_t cathodes[2*N] = {c0,c1,d1,b0,b3,d6,d5,c3, b4,d4,b1,d0,b2,b6,c2,d7};

volatile uint32_t speaker = 0;
volatile uint32_t speaker_delay = 100;
volatile uint32_t note = 0;
volatile uint32_t noteindex = 0;
volatile uint32_t notecounter = 0;
volatile uint32_t speaker_state = 0;
volatile uint8_t  rowscan = 0;
volatile uint8_t  getbuttons = 0;

volatile uint32_t graphics_state = 0;
volatile uint32_t graphics_mask = 0;
volatile uint32_t cathodes_on;
volatile uint32_t anodes_on;
volatile uint32_t metatick=0;
volatile uint32_t megametatick=0;

volatile uint8_t sound_on   = 1;
volatile uint8_t play_blues = 0;
volatile uint8_t play_up    = 0;
volatile uint8_t play_down  = 0;


volatile uint32_t speed = INITIALSPEED;
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

void newBlock() {}

void setBlock(int8_t r,int8_t c,uint8_t orientation,uint8_t *shape,uint8_t state)
{
	uint8_t i,rr;
	for (i=0;i<4;i++) {
		int8_t br = (shape[i]&15)-2, bc = shape[i]>>4;
		for(rr=orientation;rr;rr--) { int8_t temp = br; br = -bc; bc = temp; }
		br += r; bc += c;
		if (br>=0 && br<16 && bc>=0 && bc<8) set(disp,br,bc,state);
	}
}

uint8_t can_place_block(int8_t r,int8_t c,uint8_t orientation,uint8_t *shape)
{
	uint8_t i,rr;
	for (i=0;i<4;i++) {
		int8_t br = (shape[i]&15)-2, bc = shape[i]>>4;
		for(rr=orientation;rr;rr--) { int8_t temp = br; br = -bc; bc = temp; }
		br += r; bc += c;
		if (br>=16 || bc<0 || bc>=8 || (br>=0 && get(disp,br,bc))) return 0;
	}
	return 1;
}

int16_t button_debounce[4] = {0,0,0,0};
uint8_t getButtons() {
	uint8_t i;
    uint8_t buttons = getbuttons;
	for (i=0;i<4;i++)
	{
		uint8_t button = (buttons>>i)&1;
		if (button_debounce[i]>0) {
			if (button) {
				button_debounce[i]--;
				buttons &= ~(1<<i);
			} else 
				button_debounce[i]=-20;
		}
		else if (button_debounce[i]<0) {
			if (!button) button_debounce[i] = button_debounce[i] + 1;
			else button_debounce[i]=200;
		} 
		else if (button) button_debounce[i]=200;
		
	}
	return buttons;
}

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
	if (sound_on)
	{
	    if (speaker_delay) speaker_delay--;
	    else {
		    speaker_state ^= speakerpin;
       		speaker_delay = note;
       	}
        if (notecounter) notecounter--;
	   	else {
			noteindex++;
			if (play_blues)
			{
				if (noteindex>=BLUES_NNOTES) {
					noteindex=0;
					play_blues=0;
				} else {
					uint16_t info = BLUES_unique[pgm_read_byte_near(&BLUES_notes[noteindex])];
					note = BLUES_pitches[info&0xff];
					notecounter = 2500+300*noteindex;
				}
			} 
			else 
			if (play_up)
			{
				if (noteindex>=CHROM_UP_NNOTES) {
					noteindex=0;
					play_up=0;
				} else {
					uint16_t info = CHROM_UP_unique[pgm_read_byte_near(&CHROM_UP_notes[noteindex])];
					note = CHROM_UP_pitches[info&0xff];
		            notecounter = 1500;
				}
			} 
			else 
			if (play_down)
			{
				if (noteindex>=CHROM_DOWN_NNOTES) {
					noteindex=0;
					play_down=0;
				} else {
					uint16_t info = CHROM_DOWN_unique[pgm_read_byte_near(&CHROM_DOWN_notes[noteindex])];
					note = CHROM_DOWN_pitches[info&0xff];
		            notecounter = 900;
				}
			} 
			else 
			{
				if (noteindex>=NNOTES) noteindex=0;
				uint16_t info = unique[pgm_read_byte_near(&notes[noteindex])];
				note = pitches[info&0xff];
		        notecounter = (uint32_t)(info&0xff00)*5;
			}
		}
	}
   	metatick++;
   	if (metatick==30) metatick=0;
   	if (!metatick) {
		graphics_state = ((uint32_t)1)<<anodes[rowscan];
	   	graphics_mask = disp[rowscan] | graphics_state | speakerpin;
		rowscan=(rowscan+1)&15;

       	megametatick++;
       	if (megametatick==32) megametatick=0;
       	if (!megametatick) {
            PORTB = 0xDf;        
            PORTC = 0xff;
            DDRB  = 0x20;
            DDRC  = 0x00;
            delay(50);
            getbuttons = ~(((PINB&(16+8))>>3)|((PINC&(32+16))>>2));
            delay(50);
        }
	}
	setPort(graphics_state | speaker_state );
	setDDR(graphics_mask);
}

uint32_t isrow(uint8_t row)
{
	uint8_t j=0;
	for (j=0;j<8;j++)
		if (!get(disp,row,j)) return 0;
	return 1;
}

uint32_t isdead()
{
	uint8_t j=0;
	for (j=0;j<8;j++) if (get(disp,0,j)) return 1;
	return 0;
}

void slidedown(uint8_t row)
{
	uint8_t i,j;
	for (i=row;i>0;i--) for (j=0;j<8;j++) set(disp,i,j,get(disp,i-1,j));
}

void clearscreen() 
{
	uint8_t i,j;
	for (i=0;i<16;i++) for (j=0;j<8;j++) set(disp,i,j,0);
}

int main()
{    
    OSCCAL = 0xff; //oh dear. overclocking
    
    uint32_t i,j;
	int32_t ii = 0;
	
    TIMSK0 = 2; // Timer CompA interupt 1
    TCCR0B = 2; // speed
    TCCR0A = 2; // CTC mode
    OCR0A  = 36;// period
    
    //OSCCAL = 0xff; //oh dear. overclocking

    
    /* LIGHT CALIBRATION CODE DO NOT DELETE
	DDRB=DDRC=DDRD=0xff;
	while(1) {
	    uint8_t r;
	    for (r=0;r<2*N;r++) {
			i = anodes[r];
			setPort(((uint32_t)1)<<i);
			delay(1000000);
		}
		for (r=0;r<2*N;r++) {
			i = cathodes[r];
			setPort(~(((uint32_t)1)<<i));
			delay(1000000);
		}
	}
	*/
	
	/* BUTTON CALIBRATION
	while(1)
	{
        PORTB=PORTC=PORTD=0;
   	    DDRB=DDRC=DDRD=0xff;
        delay(100);
	    uint8_t r,i;
        DDRB  = 0x20;
        DDRC  = 0x00;
        PORTB = 0xDf;        
        PORTC = 0xff;
        delay(100);
        uint8_t pinb = ~(((PINB&(16+8))>>3)|((PINC&(32+16))>>2));
                
        PORTB=PORTC=PORTD=0;
   	    DDRB=DDRC=DDRD=0xff;
        delay(100);
		    for  (r=0;r<4;r++)
		        if ((pinb>>r)&1) {
		        i = cathodes[r];
		        setPort(~(((uint32_t)1)<<i));
        delay(100);
		}
	}
	*/
	
    sei();
    
	sound_on = 1;
	uint32_t score=0;
	while(1) {
		int8_t r = -4,c = 3, o = 0;
		uint8_t *block = shapes[rintn()%7];
		while(1) {
		    wait=10000;
		    while (wait)
	        {
			    uint8_t buttons = getButtons();
			    if (buttons&11) {
				    uint8_t newrow    = r;
				    uint8_t newcolumn = c;
				    uint8_t neworient = o;
				    if (buttons&2) newcolumn++;
				    if (buttons&8) newcolumn--;
				    if (buttons&1 && block!=the_o) neworient++;
				    setBlock(r,c,o,block,0);
				    if (can_place_block(newrow,newcolumn,neworient,block)) setBlock(r=newrow,c=newcolumn,o=neworient,block,1);
			    }
			    else if (buttons&4)
			    { 
				    uint16_t oldnoteindex = noteindex;
				    play_down = 1;
				    noteindex = 0;
				    while(1) {
					    uint8_t newrow = r+1;
					    setBlock(r,c,o,block,0);
					    if (can_place_block(newrow,c,o,block)) 
						    setBlock(r=newrow,c,o,block,1);
					    else
						    break;
				    }
				    noteindex = CHROM_DOWN_NNOTES-1;
				    while(play_down);
				    noteindex = oldnoteindex;
				    delay(5900);
			    }
			}
		    while (wait);
			setBlock(r,c,o,block,0);
			if (can_place_block(r+1,c,o,block)) setBlock(++r,c,o,block,1);
			else {
				setBlock(r,c,o,block,1);
				uint8_t newscore=0;
				for (i=0;i<16;) 
					if (isrow(i)) {
						uint16_t oldnoteindex = noteindex;
						play_up = 1;
						noteindex = 0;
						slidedown(i);
						newscore++;
						while(play_up);
						noteindex = oldnoteindex;
					}
					else i++;
				if (newscore) {
					score += newscore;
  					drawDecimal(score);
					delay(10000);
					clearDecimal(score);
					//speed = speed*240>>8;
					if (speed < 50 ) speed = 50;
				}
				if (isdead()) {
					play_blues = 1;
					noteindex = 0;
					while(play_blues);
					score = 0;
					speed = INITIALSPEED;
					sound_on = 0;
					while (!(0xf&getButtons()));
					clearscreen();
					sound_on = 1;
				}
				break;
			}
		}
	}
} 

