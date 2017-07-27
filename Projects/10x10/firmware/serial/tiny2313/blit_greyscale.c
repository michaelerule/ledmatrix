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


to check serial,y
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

#include <avr/io.h>
#define N 10
#define DATA_AVAILABLE (UCSRA&(1<<RXC))
#define OK_TO_SEND (UCSRA & (1 << UDRE))
#define NOP __asm__("nop\n\t")
#define once(condition) {while(!(condition));}

int main()
{	
    UBRRL = 0x1A;                // Set the baud rate 
    UCSRB = 1<<RXEN  | 1<<TXEN ; // Enable UART receive transmit 
    UCSRC = 1<<UCSZ1 | 1<<UCSZ0; // 8 data bits, 1 stop bit 
    
    uint32_t display_memory[(N+1)*2];
    uint32_t *disp = &display_memory[0];
    uint32_t *buff = &display_memory[N+1];
    uint32_t *temp;
    uint32_t pin=0, ddr=0, rx_n=0, mask=0;
    uint8_t  rx_i=0, d=0, i=0, j=0, k=0, x=0, w_i=0, r_int=0;
    
    for (i=0;i<2*(N+1);i++) display_memory[i]=0xfffffff;
    
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
        if (i>=5) pin<<=5;
        ddr = 0x1F&ddr|0xfc00&(ddr<<5)|pin;
        if (i!=N) pin=~pin;
        pin &= ddr;
        DDRA = DDRB = DDRD = 0;
        PORTB = pin;     DDRB = ddr;
        PORTD = pin>>8;  DDRD = ddr>>8;
        PORTA = pin>>15; DDRA = ddr>>15;
        if (DATA_AVAILABLE)
        {
            x = UDR;
            once(OK_TO_SEND) UDR = x;
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



