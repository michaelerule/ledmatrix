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

The Atmega168 uses 6 different registers when performing analogue to digital conversion. These are:

Register Description
ADMUX    ADC Multiplexer Selection Register
ADCSRA   ADC Control and Status Register A
ADCSRB   ADC Control and Status Register B
DIDR0    Digital Input Disable Register 0
ADCL     ADC Data Register – Low
ADCH     ADC Data Register – High

The ADMUX register allows you to control:
    The Reference Voltage
    Left adjustment of results (used for 8 bit results)
    Selection of input channel

bit             7        6        5        4    3       2       1       0
ADMUX           REFS1    REFS0    ADLAR    -    MUX3    MUX2    MUX1    MUX0
Read/Write      R/W      R/W      R/W      R    R/W     R/W     R/W     R/W
Initial Value   0        0        0        0    0       0       0       0

The reference voltage is controlled by REFS1 and REFS0. The default is to use AREF, but other options are available.
ADLAR is used for left shifting the converted data. This is useful when reading 8 bit values due to the fact that reading a 16 bit value is not an atomic operation.
MUX0 to MUX3 are used to select which input channel you wish to read. The values 0000 to 0101 allow you to select ports PC0 to PC5, while the reserved values of 1110 and 1111 allow you to select 1.1V and 0V.


ADCSRA and ADCSRB are used to control the AD conversion process.

bit              7     6    5     4    3    2     1     0
ADCSRA           ADEN  ADSC ADATE ADIF ADIE ADPS2 ADPS1 ADPS0
Read/Write       R/W   R/W  R/W   R/W  R/W  R/W   R/W   R/W
Initial Value    0     0    0     0    0    0     0     0

bit              7    6    5    4    3    2     1     0
ADCSRB           -    ACME -    -    -    ADTS2 ADTS1 ADTS0
Read/Write       R    R/W  R    R    R    R/W   R/W   R/W
Initial Value    0    0    0    0    0    0     0     0

These 2 registers provide for many different options and we will look at a subset in this tutorial.

ADEN enables the AD converter subsystem. This bit needs to be set before any conversion takes place.
ADSC is set when you want to start an AD conversion process. When the conversion is finished, the value reverts back to 0.
ADIF is set when the conversion is complete and the data is written to the result registers (ADCL/ADCH). To clear it back to 0 you need to write a 1 to it.

When an analog sample is read, a timeslice is used. The width of that timeslice is determined by the input clock. 
ADPS0 to ADPS2 sets the division factor between the system clock and the input clock.


DIDR0 is used to disable the digital input buffers on PC0 to PC5. When set, the corresponding PINC value will be set to 0.

bit              7    6    5     4     3     2     1     0
DIDR0            -    -    ADC5D ADC4D ADC3D ADC2D ADC1D ADC0D
Read/Write       R    R    R/W   R/W   R/W   R/W   R/W   R/W
Initial Value    0    0    0     0     0     0     0     0

Lastly we have the ADC Data Registers, ADCL and ADCH.
The AD converter returns a 10 bit value which is stored in these 2 registers.
The structure of these registers depends on the ADLAR value.

ADLAR=0

bit              7    6    5    4    3    2    1    0
ADCH             -    -    -    -    -    -    ADC9 ADC8
Read/Write       R    R    R    R    R    R    R    R
Initial Value    0    0    0    0    0    0    0    0

bit              7    6    5    4    3    2    1    0
ADCL             ADC7 ADC6 ADC5 ADC4 ADC3 ADC2 ADC1 ADC0
Read/Write       R    R    R    R    R    R    R    R
Initial Value    0    0    0    0    0    0    0    0

ADLAR=1

bit              7    6    5    4    3    2    1    0
ADCH             ADC9 ADC8 ADC7 ADC6 ADC5 ADC4 ADC3 ADC2
Read/Write       R    R    R    R    R    R    R    R
Initial Value    0    0    0    0    0    0    0    0

bit              7    6    5    4    3    2    1    0
ADCL             ADC1 ADC0 -    -    -    -    -    -
Read/Write       R    R    R    R    R    R    R    R
Initial Value    0    0    0    0    0    0    0    0

To configure ongoing reads of internal band gap voltage relative to the battery supply in 8 bit format?

ADMUX bits REFS1 and REFS0 default to AREF which is pulled to Vcc in the circuit. We will keep this default 
ADMUX bit ADLAR will be set to 1 so that we can just read the 8 most significant bits
ADMUX bits MUX3-0 will be set to 1110 in order to read the internal band gap 

The value of ADMUX then is 00101110 = 0x2E


ADCSRA bit ADEN should be enabeled to turn on ADC
ADCSRA bit will be set to 1 to start conversion, we will wait until it returns to 0
ADCSRA bit ADIF goes high when data is ready and we need to write a 1 to reset it

ADCSRA bits ADPS0 to ADPS2 have something to do with accuract

ADMUX  |= 0x20; //set to move the 8 most significant bits into ADCH 
ADMUX  |= 0xE;  //set to read internal 1.1V band gap
ADCSRA |= 0x80; //set ADEN bit to 1 to enable ADC
ADCSRA |= 0x40; //set ADSC to start conversion
while (ADCSRA&0x40); // wait for ADSC to clear indicating a complete conversion
uint8_t read = ADCH; // get an 8 bit ADC value
ADCSRA |= 0x10; //write a 1 to ADIF to clear the conversion-complete flag


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

    cli();
    DDRB  = 0x20;
    DDRC  = 0x00;
    PORTB = 0x18;
    PORTC = 0x30;
    delay(80);
    getbuttons = ~(((PINB&(16+8))>>3)|((PINC&(32+16))>>2));
    sei();   
    
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
    speaker_state=0;
    metatick++;
    if (metatick==4) metatick=0;
    if (!metatick) 
    {
        graphics_state = ((uint32_t)1)<<anodes[rowscan];
        graphics_mask = disp[rowscan] | graphics_state | speakerpin;
        rowscan=(rowscan+1)&15;
        /*
        megametatick++;
        if (megametatick==32) megametatick=0;
        if (!megametatick) {
            DDRB  = 0x20;
            DDRC  = 0x00;
            PORTB = 0x18;
            PORTC = 0x30;
            delay(40);
            getbuttons = ~(((PINB&(16+8))>>3)|((PINC&(32+16))>>2));
        }
        */
    }
    setDDR(0);
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

uint8_t bandGap1v1(void) // Returns actual value of Vcc (x10)
{
    ADCSRA |= 0x80; //set ADEN bit to 1 to enable ADC
    ADMUX  |= 0x20; //set to move the 8 most significant bits into ADCH 
    ADMUX  |= 0x0E; //set to read internal 1.1V band gap
    delay(2000);
    ADCSRA |= 0x40; //set ADSC to start conversion
    while (ADCSRA&0x40); // wait for ADSC to clear indicating a complete conversion
    uint8_t read = ADCH; // get an 8 bit ADC value
    ADCSRA |= 0x10; //write a 1 to ADIF to clear the conversion-complete flag
    // compute VCC in tenths of a volt
    // VCC*read/256=1.1V
    // VCC=1.1V*256/read
    // VCC=11*256/read
    // VCC=2816/read
    // VCC=uint8_t)((uint16_t)2816/(uint16_t)read)
    //return (uint8_t)((uint16_t)2816/(uint16_t)read);
    //return (uint8_t)((uint16_t)10325/(uint16_t)read);
    //return (uint8_t)((uint16_t)25813/(uint16_t)read)-150;
    return read;
}

int main()
{    
    //OSCCAL = 0x7f; //oh dear. overclocking

    uint32_t i,j;
    int32_t ii = 0;
    
    TIMSK0 = 2; // Timer CompA interupt 1
    TCCR0B = 2; // speed
    TCCR0A = 2; // CTC mode
    OCR0A  = 40;// period
    
    //button calibration II
    
    while(1)
    {
        DDRB  = 0x20;
        DDRC  = 0x00;
        PORTB = 0x18;
        PORTC = 0x30;
        delay(80);
        uint8_t buttons = ~(((PINB&(16+8))>>3)|((PINC&(32+16))>>2));
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
	}
    
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
    
    /*
    uint32_t smoothed = 0;
    uint32_t k        = 63;
    while(1)
    {
        uint8_t avcc = bandGap1v1();
        smoothed = (smoothed*(63-k)+avcc*(1+k)*16)/64;
        drawDecimal(smoothed/16);
        delay(60);
        if (k) k--;
    }
    */
    
    uint32_t score=0;
    while(1) {
        int8_t r = -4,c = 3, o = 0;
        uint8_t *block = shapes[rintn()%7];
        while(1) {
            /*
            if (bandGap1v1()>175)
            {
                clearscreen();
                uint8_t lowmsg[15] = {
                0x00,0x10,0x20,0x30,
                0x40,0x41,0x42,0x13,
                0x23,0x33,0x44,0x04,
                0x15,0x25,0x35};
                uint8_t i=0;
                for (i=0;i<15;i++)
                {
                    uint8_t d = lowmsg[i];
                    flip(disp,d>>4,d&15);
                }
            }
            */
            wait=100;
            while (wait--)
            {
                uint8_t buttons = getButtons();
                if (buttons&11) {
                    uint8_t newcolumn = c;
                    uint8_t neworient = o;
                    if (buttons&2) newcolumn++;
                    if (buttons&8) newcolumn--;
                    if (buttons&1 && block!=the_o) neworient++;
                    setBlock(r,c,o,block,0);
                    if (can_place_block(r,newcolumn,neworient,block)) setBlock(r,c=newcolumn,o=neworient,block,1);
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
                }
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
                    delay(60000);
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

