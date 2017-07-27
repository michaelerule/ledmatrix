/*
Hardware notes
============================================================================

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

run this to make it go 8x as fast : 
avrdude -c avrispmkII -p t2313 -B100 -P /dev/ttyUSB0 -U lfuse:w:0xE2:m 

run this to change it back :
avrdude -c avrispmkII -p t2313 -B100 -P /dev/ttyUSB0 -U lfuse:w:0x62:m 

to check serial,
-- set up to listen
-- listen in screen
-- set firmware to scan baudrate
-- wait for something sane to appear in screen
-- exit
-- use this baudrate

stty -F /dev/ttyUSB1 9600 cs8 cread
stty -F /dev/ttyUSB0 9600 cs8 cread
screen /dev/ttyUSB1
C-a : quit

============================================================================
*/
#define NOP __asm__("nop\n\t")
#include <avr/io.h>
#define N 10
#define SCANRATE 28
#define colordepth 2
#define framebytes (2*N)
#define DATA_AVAILABLE (UCSRA&(1<<RXC))
#define OK_TO_SEND (UCSRA & (1 << UDRE))

int main()
{	
    uint16_t disp[(N+1)*colordepth];
    uint8_t data_index = 0;
    uint8_t data_frame = 0;
    UBRRL = 0x1A;                // Set the baud rate 
    UCSRB = 1<<RXEN  | 1<<TXEN ; // Enable UART receive transmit 
    UCSRC = 1<<UCSZ1 | 1<<UCSZ0; // 8 data bits, 1 stop bit 
	while (1) 
    {
    	uint8_t scanD;
        for (scanD=0; scanD<colordepth; scanD++)
        {
        	uint16_t k;
            for (k=(1<<(colordepth-1))>>scanD; k>0; k--) {
                uint8_t scanI;
                for (scanI=0; scanI<=N; scanI++)
                {
                    uint16_t *d = &disp[(N+1)*scanD];
                    uint32_t pin = 1<<scanI;
                    if (scanI>=5) pin<<=5;
                    uint32_t ddr = d[scanI];
                    ddr = 0x1F&ddr|0xfc00&(ddr<<5)|pin;
                    if (scanI!=N) pin=~pin;
                    pin &= ddr;
                    PORTB = pin;     DDRB = ddr;
                    PORTD = pin>>8;  DDRD = ddr>>8;
                    PORTA = pin>>15; DDRA = ddr>>15;
                    if (DATA_AVAILABLE)
                    {
                        uint8_t data = UDR;
                        if (data==0x40) 
                        {
                            data_index = data_frame = 0;
                        }
                        else
                        {
                            uint16_t *d = &disp[(N+1)*data_frame];
                            uint8_t  r = data_index/2;
                            uint8_t  x = data_index&1? 5 : 0;
                            uint8_t  i;
                            for (i=0;i<5;i++) 
                            {
                                uint8_t c = i+x;
                                if ((d[r==c?N:c]>>r&1)!=((data>>i)&1)) d[r==c?N:c]^=1<<r;
                            }
                            if(++data_index>=framebytes) {
                                data_index=0;
                                if (++data_frame>=colordepth) {
                                    data_frame=0;
                                }
                            }
                        }
                    }
                }
            }
        }
	}
} 



