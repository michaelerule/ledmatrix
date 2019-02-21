/*
Hardware notes
===============================================================================

-U lfuse:w:0xe4:m

DDRx   : 1 = output, 0 = input
PORTx : output buffer
PINx  : digital input buffer ( writes set pullups )
                          ______
         !RESET     PC6 -|  U  |- PC5
                    PD0 -|     |- PC4
                    PD1 -|     |- PC3
                    PD2 -|     |- PC2
                    PD3 -|  m  |- PC1
                    PD4 -|  *  |- PC0
                    VCC -|  8  |- GND
                    GND -|     |- AREF
                    PB6 -|     |- AVCC
                    PB7 -|     |- PB5   SCK  ( yellow )
                    PD5 -|     |- PB4   MISO ( green )
                    PD6 -|     |- PB3   MOSI ( blue )
                    PD7 -|     |- PB2
                    PB0 -|_____|- PB1

        Programmer pinout, 6 pin:
                o
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

avrdude -c avrispmkII -p m48 -B100 -P /dev/ttyUSB0 -U lfuse:w:0xE2:m 
avrdude -c avrispmkII -p m48 -B1000 -P /dev/ttyUSB0 -F

D3456 are anodes
B0..5 and also C01 are the cathodes

1MHz clock (RC oscillator)
What should we use 
*/

/*

Protocol for 7 segment display:

Lower 128 chars behave like ASCII
Triggering respective character lookups.
Select characters will map to readable glyphs.
Remaining characters will map to approximate glyphs or nothing at all.

upper characters trigger direct edit


*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#define N 8
#define ONE ((uint32_t)1)
#define DATA_AVAILABLE (UCSRA&(1<<RXC))
#define OK_TO_SEND (UCSRA & (1 << UDRE))
#define NOP __asm__("nop\n\t")
#define once(condition) {while(!(condition));}
#define READ(x) ((uint8_t)(pgm_read_byte_near(&x)))

uint8_t ASCII[128]={
0b0000000,0b1011110,0b0011000,0b1000100,0b0101110,0b0101110,0b0010111,
0b0001000,0b0001100,0b0100010,0b1011100,0b1001001,0b0100010,0b1000000,
0b0100000,0b0010001,0b0111111,0b0110000,0b0110110,0b1110110,0b1111000,
0b1101110,0b1101111,0b0110100,0b1111111,0b1111100,0b0000110,0b0100110,
0b0000011,0b1000010,0b0001100,0b1010101,0b0011111,0b1111101,0b1111111,
0b0001111,0b0110110,0b1001111,0b1001101,0b0101111,0b1111001,0b0001001,
0b0110011,0b1011011,0b0001011,0b0100101,0b0111101,0b0111111,0b1011101,
0b1011110,0b1110101,0b1101110,0b0001101,0b0111011,0b0111011,0b0011010,
0b1000110,0b1111010,0b1010111,0b0001110,0b0101000,0b0110110,0b0011100,
0b0000010,0b0010000,0b1110111,0b1101011,0b1000011,0b1110011,0b1011111,
0b1001101,0b1111110,0b1101001,0b0000001,0b0100010,0b1100010,
0b0110000,0b1100101,0b1100001,0b1100011,0b1011101,
0b1111100,0b1000001,0b1101000,
0b1001011,0b0100011,0b0100011,
0b0100111,0b0100001,0b1011001,0b1010001,
0b1110000,0b1001000,0b0111001,0b0000100};

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

void delay(uint32_t n)
{
    while (n--) NOP;
}

//D3456 are anodes
//B0..5 and also C01 are the cathodes
//PIN POSITIONS ON DISPLAY
const uint8_t anodes[4]   PROGMEM = {6,5,4,3};
const uint8_t cathodes[8] PROGMEM = {8,9,16,17,18,19,20,21};

#define baudrate (1000000/(9600*16-1))

int main()
{   
    // ENABLE SERIAL for 8MHz clock
    UCSRB = 0x18;
    UCSRC = 0x86;
    UBRRL = 0x33;

    uint8_t r,c,i;
    uint8_t disp[4] = {0x55,0x55,0x55,0x55};
    uint8_t buff[4] = {0x55,0x55,0x55,0x55};
    uint8_t scroll_mode = 1;
    
    while(1)
    for (r=0;r<4;r++)
    for (c=0;c<8;c++) {
        // SCAN DISPLAY
        uint32_t C = ONE<<READ(cathodes[c]);
        uint32_t A = ONE<<READ(anodes[r]);
        setDDR(0);
        setPort(A);
        if ((disp[r]>>c)&1) 
        	setDDR(C|A);
        delay(10);
        
        //HANDLE NEW DATA IF PRESENT
        if (DATA_AVAILABLE)
        {
            uint8_t x = UDR;
            if (x<32)
            {
            	switch (x)
            	{
					case 17:
		        		scroll_mode = 0;
		        		for (i=0;i<4;i++)
		        			buff[i]=disp[i];
	        			break;
        			case 18:
		        		scroll_mode = 1;
        			case 19:
		        		for (i=0;i<4;i++)
		        			disp[i]=buff[i];
	        			break;
            	}
            }
            else if (x>=32 && x<127)
            {
            	x-=32;
            	x=ASCII[x];
            	x=(x & 0b11111)|((x & 0b1100000)<<1);
            	uint8_t *working = scroll_mode? &disp[0]:&buff[0];
			    once(OK_TO_SEND) 
			        UDR = working[3]; // SDO
			    for (i=3;i>=1;i--)
			        working[i]=working[i-1];
			    working[0]=x;
            }
            else
            {
	            x &= 0x7f;
            	x=(x & 0b11111)|((x & 0b1100000)<<1);
            	uint8_t *working = scroll_mode? &disp[0]:&buff[0];
			    once(OK_TO_SEND) 
			        UDR = working[3]; // SDO
			    for (i=3;i>=1;i--)
			        working[i]=working[i-1];
			    working[0]=x;
            }
        }
    }
} 






