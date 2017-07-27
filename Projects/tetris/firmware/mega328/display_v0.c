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


Pins in use for display : 

B 7 5 4 3 2 1 0
C 5 4 3 2 1 0
D 7 6 5

scan order B C D

B0 00
B1 01
B2 02
B3 03
B4 04
B5 05
B6 06
B7 07

C0 08
C1 09
C2 0A
C3 0B
C4 0C
C5 0D
C6 0E
C7 0F

D0 10
D1 11
D2 12
D3 13
D4 14
D5 15
D6 16
D7 17


Matrix connections

A C5 0d  02 B2 A
C C4 0c  01 B1 A
C C3 0b  03 B3 C
A C2 0a  15 D5 A
C C1 09  07 B7 C
A C0 08  16 D6 C
C B5 05  17 D7 A
C B4 04  00 B0 A

B3
B2
B1
B7
D5
D6
D7
B0

0x0d, 0x0a, 0x08, 0x02, 0x01, 0x15, 0x17, 0x00
0x0c, 0x0b, 0x09, 0x05, 0x04, 0xb3, 0xb7, 0xd6

3
1
4
2
5
7

0 3
1 -
2 4
3 2
4 5
5 -
6 6
7 -

274---56

============================================================================*/



/*

cathode probe sequence

0x00	8
0x01	10
0x02	12
0x03	3
0x04	0
0x05	1
0x06	-
0x07	5
0x08	14
0x09	7
0x0A	13
0x0B	2
0x0C	4
0x0D	11
0x0E	-
0x0F	-
0x10	-
0x11	-
0x12	-
0x13	-
0x14	15
0x15	6
0x16	9

anode probe seqence

0x00	4
0x01	5
0x02	2
0x03	8
0x04	12
0x05	14
0x06	-
0x07	11
0x08	1
0x09	15
0x0A	3
0x0B	13
0x0C	10
0x0D	0
0x0E	-
0x0F	-
0x10	-
0x11	-
0x12	-
0x13	-
0x14	7
0x15	9
0x16	6


cathode probe sequence

0x04	0
0x05	1
0x0B	2
0x03	3
0x0C	4
0x07	5
0x15	6
0x09	7
0x00	8
0x16	9
0x01	10
0x0D	11
0x02	12
0x0A	13
0x08	14
0x14	15

anode probe seqence

0x0D	0
0x08	1
0x02	2
0x0A	3
0x00	4
0x01	5
0x16	6
0x14	7
0x03	8
0x15	9
0x0C	10
0x07	11
0x04	12
0x0B	13
0x05	14
0x09	15




cathode probe sequence

0x04,0x05,0x0B,0x03,0x0C,0x07,0x15,0x09,0x00,0x16,0x01,0x0D,0x02,0x0A,0x08,0x14
0x0D,0x08,0x02,0x0A,0x00,0x01,0x16,0x14,0x03,0x15,0x0C,0x07,0x04,0x0B,0x05,0x09
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#define N 8
#define NOP __asm__("nop\n\t")
#include <avr/pgmspace.h>
#include "scale.c"
#include "blues.c"
#include "chromatic_up.c"
#include "chromatic_down.c"

#define speakerpin ((uint32_t)1<<20)

#define LEFT   16
#define RIGHT  18
#define ROTATE 17
#define DOWN   19
#define LEFTPIN   (((uint32_t)1)<<(LEFT))
#define RIGHTPIN  (((uint32_t)1)<<(RIGHT))
#define ROTATEPIN (((uint32_t)1)<<(ROTATE))
#define DOWNPIN   (((uint32_t)1)<<(DOWN))
#define button_pull_up_mask (LEFTPIN|RIGHTPIN|ROTATEPIN|DOWNPIN)
#define button_mask (~button_pull_up_mask)

#define INITIALSPEED 100

uint8_t the_1[4] = {0x00,0x01,0x02,0x03};
uint8_t the_L[4] = {0x01,0x02,0x03,0x13};
uint8_t the_J[4] = {0x01,0x11,0x02,0x03};
uint8_t the_o[4] = {0x02,0x12,0x03,0x13};
uint8_t the_s[4] = {0x01,0x02,0x12,0x13};
uint8_t the_z[4] = {0x11,0x02,0x12,0x03};
uint8_t the_t[4] = {0x01,0x02,0x12,0x03};
uint8_t* shapes[7] = {the_L,the_1,the_J,the_o,the_s,the_z,the_t};

uint8_t cathodes[2*N] = { 0x04,0x05,0x0B,0x03, 0x0C,0x07,0x16,0x09,    0x00,0x17,0x01,0x0D, 0x02,0x0A,0x08,0x15 };
uint8_t anodes  [2*N] = { 0x0D,0x08,0x02,0x0A, 0x00,0x01,0x17,0x15,    0x03,0x16,0x0C,0x07, 0x04,0x0B,0x05,0x09 };

volatile uint32_t speaker = 0;
volatile uint32_t speaker_delay = 100;
volatile uint32_t note = 0;
volatile uint32_t noteindex = 0;
volatile uint32_t notecounter = 0;
volatile uint32_t speaker_state = 0;
volatile uint8_t rowscan = 0;

volatile uint32_t graphics_state = 0;
volatile uint32_t graphics_mask = 0;
volatile uint32_t cathodes_on;
volatile uint32_t anodes_on;
volatile uint32_t metatick=0;

volatile uint32_t speed = INITIALSPEED;

uint32_t b0[16],b1[16];
uint32_t *disp = &b0[0];
uint32_t *buff = &b1[0];

volatile uint8_t sound_on   = 1;
volatile uint8_t play_blues = 0;
volatile uint8_t play_up    = 0;
volatile uint8_t play_down  = 0;

void delay(uint32_t n) { while (n--) NOP; }

uint64_t rng = 6719;
uint32_t rintn() { return (uint32_t)(rng = ((uint64_t)rng * 279470273UL) % 4294967291UL); }

uint8_t get(uint32_t *d,uint8_t r, uint8_t c) {
	return 1&(d[r<8?c:c+8]>>cathodes[r]); 
}

void flip(uint32_t *d,uint8_t r, uint8_t c) 
{ 
	d[r<8?c:c+8] ^= ((uint32_t)1)<<cathodes[r]; 
}
void    set(uint32_t *d,uint8_t r, uint8_t c, uint8_t v) { if (get(d,r,c)!=v) flip(d,r,c); }

#include "alphabet.c"

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
	uint8_t buttons = ~PIND;
	uint8_t i;
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
    DDRB = ddr & 0xff;
    ddr >>= 8;
    DDRC = ddr & 0xff;
    ddr >>= 8;
    DDRD = ddr & 0xff;
}

void setPort(uint32_t pins)
{
    PORTB = pins & 0xff;
    pins >>= 8;
    PORTC = pins & 0xff;
    pins >>= 8;
    PORTD = pins & 0xff;
}

ISR(TIMER0_COMPA_vect) 
{
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
					notecounter = ((uint32_t)(info&0xff00)*(speed+200)*6)/300;
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
					notecounter = ((uint32_t)(info&0xff00)*(speed+200)*6)/300;
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
					notecounter = 500;
				}
			} 
			else 
			{
				if (noteindex>=NNOTES) noteindex=0;
				uint16_t info = unique[pgm_read_byte_near(&notes[noteindex])];
				note = pitches[info&0xff];
				notecounter = ((uint32_t)(info&0xff00)*(speed+200)*6)/300;
			}
		}
	}
   	metatick++;
   	if (metatick==10) metatick=0;
   	if (!metatick) {
		graphics_state = ((uint32_t)1)<<anodes[rowscan];
	   	graphics_mask = disp[rowscan] | graphics_state | speakerpin;
	   	graphics_state |= button_pull_up_mask;
		rowscan=(rowscan+1)&15;
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

void initDisplay()
{
    TIMSK0 = 2; // Timer CompA interupt 1
    TCCR0B = 2; // speed
    TCCR0A = 2; // CTC mode
    OCR0A  = 36;// period
    sei();
}

int main()
{ 	
    uint32_t i,j;
	int32_t ii = 0;
	
   	initDisplay();
   
	sound_on = 0;
	while (!(0xf&getButtons()));
	while (!(0xf&getButtons()));
	clearscreen();
	sound_on = 1;
	uint32_t score=0;
	while(1) {
		int8_t r = -3,c = 3, o = 0;
		uint8_t *block = shapes[rintn()%7];
		while(1) {
			uint32_t wait = 100;
			while(wait--) 
			{
				uint8_t buttons = getButtons();
				if (buttons&13) {
					uint8_t newcolumn = c;
					uint8_t neworient = o;
					if (buttons&1) newcolumn--;
					if (buttons&4 && block!=the_o) neworient++;
					if (buttons&8) newcolumn++;
					setBlock(r,c,o,block,0);
					if (can_place_block(r,newcolumn,neworient,block)) setBlock(r,c=newcolumn,o=neworient,block,1);
				}
				else if (buttons&2)
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
				}
				//delay(speed);
			}
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
					speed = speed*240>>8;
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

