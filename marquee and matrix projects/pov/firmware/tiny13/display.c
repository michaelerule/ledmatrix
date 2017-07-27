/* Hardware notes
=============================================================================
    DDRx   : 1 = output, 0 = input
    PORTx : output buffer
    PINx  : digital input buffer ( writes set pullups )
    Port B is the chip IO port ( other ports are control registers )

    AtTiny13 pinout:
                    __ __
        4 RST PB5 -|  U  |- VCC       3
              PB3 -|     |- PB2 SCK   5
              PB4 -|     |- PB1 MISO  6
        1     GND -|_____|- PB0 MOSI  2
                
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
        
        
    Programmer pinout, 6 pin, linear:
                
        6 MISO +  
        5 SCK  + 
        4 RST  +  
        3 VCC  +
        2 MOSI +
        1 GND  +
        
    PORT : write to here to set output
    DDR  : write to here to set IO. 1 for output.
    PIN  : pull ups ? digital input ?
===========================================================================*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#define NOP __asm__("nop\n\t")
#define N 16

const uint8_t pixel_info[] PROGMEM = {
	0,2,2,0,2,2,0,
	2,3,3,2,3,3,2,
	2,3,3,3,3,3,2,
	0,2,3,3,3,2,0,
	0,0,2,3,2,0,0,
	0,0,0,2,0,0,0,};
	
uint16_t disp[N];

void delay(uint16_t n) { while (--n) NOP; }
uint8_t get(uint8_t r, uint8_t c ) { return disp[c]>>r&1; }
void    set(uint8_t r, uint8_t c, uint8_t v) { if (get(r,c)!=v) disp[c]^=1<<r; }

int main() {    
    uint8_t i,j;
    for (i=0;i<7;i++) for (j=0;j<7;j++) set(j+2,i,pgm_read_byte_near(&pixel_info[j*7+i])==2?1:0);
    uint8_t scanI=0;
    while(1)
    {
        uint16_t data = disp[scanI];
        uint16_t k = 0;
        uint8_t  flip = 0;
        for ( k=0; k<100; k++)
        {
            uint8_t show = ( flip ? data : (data>>5) ) & 0x1f;
            DDRB  = 0;
            PORTB = (flip ? show : ~show ) & 0x1f;
            DDRB  = show;
            flip ^= 1;
            delay(20);
        }
        scanI++;
        if (scanI>=N) scanI=0;
    }
} 



