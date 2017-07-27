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

No overclocking !

USE default 1mhz system clock
want to accept 9600 and send at the same
but buffer timing so that there is 60 ms between each frame
more or less

how long is a delay of 30 milliseconds at 1MHZ? 
1,000,000 cycle per second
1,000 cycles per millisecond

so something like 30,000 NOPs
we need a dumb loop for this though.

9600 baud 9600 per second
1,000,000 cycles per second clock
send something every 104.166666667 clock cycles?
lets call it 105 since we know how to deal with that
21 nops

to build


avr-gcc -Os -mmcu=attiny2313 ./tiny2313serialbuffer.c -o buffer.o
avr-objcopy -j .text -j .data -O binary buffer.o buffer.hex
du -b buffer.hex
avrdude -c avrispmkII -p t2313 -B10 -P /dev/ttyUSB0 -U flash:w:buffer.hex

avr-objdump -d buffer.o
*/


#include <avr/io.h>
#define N 10
#define DATA_AVAILABLE (UCSRA&(1<<RXC))
#define OK_TO_SEND (UCSRA & (1 << UDRE))
#define NOP __asm__("nop\n\t")
#define once(condition) {while(!(condition));}

#define SERIALDELAY 181
#define BYTEDELAY 30000

int main()
{	
	OSCCAL = 64+32;
    UBRRL = 52;                // Set the baud rate to 9600
    UCSRB = 1<<RXEN  | 1<<TXEN ; // Enable UART receive transmit 
    UCSRC = 1<<UCSZ1 | 1<<UCSZ0; // Configure UART for 8 data bits, 1 stop bit 
    
    DDRA = 0x1; // send on pin A0
    PORTA = 0;
    
    uint8_t x;
    uint8_t i;
    uint16_t j;
    uint32_t k;
    uint8_t l=0;
    
    while(1) 
    if (DATA_AVAILABLE)
    {
    	// GET DATA AND ECHO IT
        x = UDR;
        once(OK_TO_SEND) UDR = x;
        
        //x=1<<l;
        
        // delay 30ms
    	k = BYTEDELAY;
    	while (k--) NOP;
    	
        // SEND DATA TO VIRTUAL PORT
        PORTA = 0;
		j = SERIALDELAY;
		while (j--) NOP;
        PORTA = 1;
		j = SERIALDELAY;
		while (j--) NOP;
        PORTA = 0;
		j = SERIALDELAY;
		while (j--) NOP;
        for (i=0;i<8;i++)
        {
        	PORTA = x&1;
			j = SERIALDELAY;
			while (j--) NOP;
        	x>>=1;
        }
        PORTA = 0;
		j = SERIALDELAY;
		while (j--) NOP;
        
        // delay 30ms
    	k = BYTEDELAY;
    	while (k--) NOP;
    	
    	l++;
    	l&=7;
    }
} 














