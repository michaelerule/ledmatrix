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

avrdude -c avrispmkII -p m328p -B100 -P /dev/ttyUSB0 -U lfuse:w:0xE2:m -U hfuse:w:0xD9:m -U efuse:w:0xFF:m -F
avrdude -c avrispmkII -p m328p -B100 -P /dev/ttyUSB0 -U lfuse:w:0xE2:m 
avrdude -c avrispmkII -p m328p -B1000 -P /dev/ttyUSB0 -F

micro display notes

           ___________________      2
          |                   |     2
        A | +               + | A   S
        C | +               + | A   S
        C | +               + | C   S
        A | +     [TOP]     + | A   B
        C | +               + | C   8
        A | +               + | C   8
        C | +               + | A   7
        C | +               + | A   D
          |___________________|     L

    2       ___________________      
    2      |                   |     
    S    A | +               + | A   
    S    A | +               + | C   
    S    C | +               + | C   
    B    A | +   [BOTTOM]    + | A   
    8    C | +               + | C   
    8    C | +               + | A   
    7    A | +               + | C   
    D    A | +               + | C   
    L      |___________________|     

    2       ___________________      
    2      |                   |     
    S    A | +               + | A   
    S    A | +               + | C   
    S    C | +               + | C   
    B    A | +   [BOTTOM]    + | A   
    8    C | +               + | C   
    8    C | +               + | A   
    7    A | +               + | C   
    D    A | +               + | C   
    L      |___________________|     


Physical pin order

0    0 B0
1    1 B1
2    2 B2
3    3 B3
4    4 B4
5    5 B5
6    6 B6
7    7 B7
8    0 D0 ( piezo )
9    1 D1
a    2 D2
b    3 D3
c    4 D4
d    5 D5
e    6 D6
f    7 D7
10   0 A0
11   1 A1 
12   2 A2 ( reset )

anode scan
6 4 7 5 2 0 1 3 - 12 10 8 9 11 14 - 13 15

cathode scan
15 14 8 13 11 12 10 9 - 6 3 4 2 1 7 - 5 0

anode scan
05,06,04,07,01,03,00,02,11,12,10,13,09,16,14,17

cathode scan
17,13,12,10,11,16,09,14,02,07,06,04,05,03,01,00

*/

#include <avr/io.h>
















#include <avr/pgmspace.h>
#include "alphadata.h"
#include "scale.h"
#define BYTE(x) (pgm_read_byte_near(&x))
#define speakerpin 0x100

// DELAY
void delay(uint32_t n) { while (n--) __asm__("nop\n\t"); }

/// RANDOM
uint16_t rng = 6719; 
uint16_t rintn() { return rng = rng*31421+6927 ; }

/// IO
void setDDR (uint32_t ddr ) { DDRB  = ddr ; ddr  >>= 8; DDRD  = ddr ; ddr  >>= 8; DDRA  = ddr ; }
void setPort(uint32_t pins) { PORTB = pins; pins >>= 8; PORTD = pins; pins >>= 8; PORTA = pins; }

/// DISPLAY & SOUND
uint8_t anodes[]   PROGMEM = {17,13,12,10,11,16,9,14,2,7,6,4,5,3,1,0};
uint8_t cathodes[] PROGMEM = {5,6,4,7,1,3,0,2,11,12,10,13,9,16,14,17};
uint32_t disp[16];
uint8_t get(uint8_t r, uint8_t c) { return 1&(disp[r]>>BYTE(anodes[r<8?c:c+8])); }
void   flip(uint8_t r, uint8_t c) { disp[r]^= (((uint32_t)1)<<BYTE(anodes[r<8?c:c+8])); }
void    set(uint8_t r, uint8_t c, uint8_t v) { if (get(r,c)!=v) flip(r,c); }
void initDisplay() {
    TCCR0A = _BV(COM0A0) | _BV(WGM01);
    TCCR0B = _BV(CS01);
    TIMSK |= 1<<OCIE0A;
    OCR0A = 1; 
    sei();
}
/*
volatile uint8_t rowscan = 0;
volatile uint8_t speaker_delay = 0;
volatile uint32_t speaker_state = 0;
volatile uint8_t note = 0;
volatile uint32_t notecounter = 0;
volatile uint32_t noteindex = 0;
/*
ISR(TIMER0_COMPA_vect) 
{
    if (!speaker_delay--)
    {
        speaker_state ^= speakerpin;            
   		speaker_delay  = note;
   	}
   	if (!notecounter--)
   	{
        if (++noteindex>=NNOTES) noteindex=0;
        note = BYTE(pitches[BYTE(notes[noteindex])]);
        notecounter = 3000;
    }
   	
    uint32_t cathode = (uint32_t)1<<BYTE(cathodes[rowscan]);
    setDDR(0);
    setPort( cathode | speaker_state );
    setDDR( disp[rowscan] | cathode | speakerpin );
    rowscan=(rowscan+1)&15;
}
*/
/// TETRIS
/*
uint8_t  the_1[4]  PROGMEM = {0x00,0x01,0x02,0x03};
uint8_t  the_L[4]  PROGMEM = {0x01,0x02,0x03,0x13};
uint8_t  the_J[4]  PROGMEM = {0x01,0x11,0x02,0x03};
uint8_t  the_o[4]  PROGMEM = {0x02,0x12,0x03,0x13};
uint8_t  the_s[4]  PROGMEM = {0x01,0x02,0x12,0x13};
uint8_t  the_z[4]  PROGMEM = {0x11,0x02,0x12,0x03};
uint8_t  the_t[4]  PROGMEM = {0x01,0x02,0x12,0x03};
uint8_t* shapes[7] PROGMEM = {the_L,the_1,the_J,the_o,the_s,the_z,the_t};
uint32_t isrow(uint8_t row)     { uint8_t j;   for (j=0;j<8;j++)   if (!get(row,j)) return 0; return 1;   }
uint32_t isdead()               { uint8_t j;   for (j=0;j<8;j++)   if (get(0,j))    return 1; return 0;   }
void     slidedown(uint8_t row) { uint8_t i,j; for (i=row;i>0;i--) for (j=0;j<8;j++) set(i,j,get(i-1,j)); }
void     clearscreen()          { uint8_t i,j; for (i=0;i<16;i++)  for (j=0;j<8;j++) set(i,j,0);          }
*/

int main()
{     
    uint8_t i,j,k,r,c;
    
    for (c=0;c<8;c++) for (r=0;r<16;r++)
        set(r,c,1);
    setDDR(0xffffffff);
    //initDisplay();
    
    uint8_t rowscan = 0;
    uint8_t display_delay = 0;
    uint8_t speaker_delay = 0;
    uint32_t speaker_state = 0;
    uint8_t note = 0;
    uint8_t flipper = 0;
    uint32_t notecounter = 0;
    uint32_t noteindex = 0;

	while(1) {

       	if (!display_delay--)
       	{
            uint32_t cathode = (uint32_t)1<<BYTE(cathodes[rowscan]);
            setDDR(0);
            setPort( cathode | speaker_state );
            setDDR( disp[rowscan] | cathode | speakerpin );
            rowscan=(rowscan+1)&15;
            display_delay=537;//(flipper^=1)?note>>1:note-(note>>1);
        }
    }
} 

