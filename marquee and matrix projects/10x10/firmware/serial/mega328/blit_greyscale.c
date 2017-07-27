/*
Hardware notes
============================================================================

    DDRx   : 1 = output, 0 = input
    PORTx : output buffer
    PINx  : digital input buffer ( writes set pullups )

( colors are for custome "ugly pin" adaptor )
                          __ __
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
        Programmer pinout, 6 pin:
                
        6 MISO +-+  
        5 SCK  + + 
        4 RST  +-+  
        VCC 3
        MOSI 2
        GND 1

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
1350

avrdude -c avrispmkII -p m328p -B1000 -P /dev/ttyUSB0 -F

============================================================================
*/

#include <avr/io.h>
#define N 10
#define DATA_AVAILABLE (UCSR0A&(1<<RXC0))
#define NOP __asm__("nop\n\t")
#define once(condition) {while(!(condition));}

//while(!(UCSR0A&(1<<RXC0)) ){};

uint8_t pin_numbers[11] = {0, 1, 2, 6, 7, 10, 11, 12, 13, 14, 15};

uint32_t getPinstate(uint32_t vector)
{
	uint32_t value = 0;
	uint8_t i;
	for (i=0;i<11;i++) {
		if (vector&1) value |= 1<<pin_numbers[i];
		vector >>= 1;
	}
	return value;
}

int main()
{	
    UCSR0B = 0x18;
    UCSR0C = 0x86;
    UBRR0L = 0x33;
   
    uint32_t display_memory[(N+1)*2];
    uint32_t *disp = &display_memory[0];
    uint32_t *buff = &display_memory[N+1];
    uint32_t *temp;
    uint32_t pin=0, ddr=0, rx_n=0, mask=0;
    uint8_t  rx_i=0, d=0, i=0, j=0, k=0, x=0, w_i=0, r_int=0;
    
    for (i=0;i<2*(N+1);i++) display_memory[i]=0xfffffffff;
    
    while(1) 
    for (d=0   ; d<3 ; d++) 
    for (k=1<<d; k>0 ; k--)
    for (i=0   ; i<=N; i++)
    {
        pin = 1<<i;
        ddr = disp[i];
        ddr = (i!=N)? ((ddr>>9*d)&0x1ff) : ((ddr>>10*d)&0x3ff);
        mask = (1<<i)-1;
        ddr = (ddr&mask)|((ddr&~mask)<<1);

        // this bit needs to change
        //         3     5
        // 0 1 2   6 7   10 11 12 13 14 15
        /*
        if (i>=3) {
            if (i>=5) pin<<=5;
            else      pin<<=3;
        }
        ddr = 0xffffffff;
        ddr = 0x00007&ddr|0x0018&(ddr<<3)|0xfc00&(ddr<<5)|pin;
        */
        
        pin = getPinstate(1<<i);
        ddr = getPinstate(ddr|1<<i);
        
        if (i!=N) pin=~pin;
        pin &= ddr;
        DDRC = DDRB = DDRD = 0;
        PORTB = pin;     
        PORTD = pin>>8;  
        PORTC = pin>>16; 
        DDRB = ddr;
        DDRD = ddr>>8;
        DDRC = ddr>>16;
        // this can stay the same
        if (DATA_AVAILABLE)
        {
            x = UDR0;
            if (x&0x80)
            {
                r_int = rx_i = 1;
                rx_n = x&0x7f;
            }
            else if (r_int) {
                rx_n <<=7;
                rx_n |= x&0x7f;
                if (rx_i==3)
                {
                    if (rx_n>>30==1) w_i=0;
                    buff[w_i]=rx_n&0x3FFFFFFF;
                    if (++w_i>N) {
                        w_i=0;
                        temp = disp; 
                        disp = buff; 
                        buff = temp;
                    }
                    r_int = 0;
                }
                rx_i++;
            }
        }
    }
} 



