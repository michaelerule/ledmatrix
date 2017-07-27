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
#include <avr/interrupt.h>
#include "rng.h"
#include "tinyio.h"
#include "serial.h"

#define N 10
#define NN ((N)*(N))
#define SCANRATE 28

uint16_t b0[N+1],b1[N+1];
uint16_t *disp = &b0[0];
uint16_t *buff = &b1[0];
volatile uint8_t scanI = 0;

volatile uint8_t data_index = 0;

uint8_t get(uint8_t r, uint8_t c,uint16_t *d)  {
	return d[r==c?N:c]>>r&1;
}

void set(uint8_t r,uint8_t c,uint8_t v,uint16_t *d)  {
	if (get(r,c,d)!=v) d[r==c?N:c]^=1<<r;
}

void flipBuffers() {
	uint16_t *temp = buff;
	buff = disp;
	disp = temp;
}

void handle_serial() 
{
    uint8_t data;
    if (maybe_read(&data))
    {
        while(!maybe_send(data));
        if (data == 0x80) // high bits mean reset
            data_index = 0;
        else if (data == 0x40) // high bits mean reset
            flipBuffers();
        else
        {
            uint8_t buffer_index = data_index>>1;
            uint8_t i;
            uint8_t x = data_index & 1 ? 5 : 0;
            for (i=0;i<5;i++) 
                set(buffer_index,i+x,data>>i&1,buff);
            data_index = (data_index+1 % 20);
        }
    }
}

uint8_t pins[11] = {0,1,2,3,4,10,11,12,13,14,15};

uint32_t getPinstate(uint32_t vector)
{
	uint32_t value = 0;
	uint8_t i;
	for (i=0;i<11;i++) {
		value |= (vector&1)<<pins[i];
		vector >>= 1;
	}
	return value;
}

void scan_display()
{
	uint32_t pin = 1<<pins[scanI];
	uint32_t ddr=getPinstate(disp[scanI]);
	ddr |= pin;
	if (scanI!=N) pin=~pin;
	pin &= ddr;
	setDDR(0);
	setPort(pin);
	setDDR(ddr);
	if (++scanI>N) scanI=0;
}

ISR(TIMER1_COMPA_vect)  {
    scan_display();
    handle_serial();
}

void timer1(void) 
{ 
    OCR1A = SCANRATE;               //Set OCR1A 
    TCCR1A = 0x00;                //Timer counter control register 
    TCCR1B = (0 << WGM13)|(1 << WGM12)|(0 << CS12)|(1 << CS11)|(1<< CS10); // WGM1=4, prescale at 1024 
    TIMSK |= (1 << OCIE1A);         //Set bit 6 in TIMSK to enable Timer 1 compare interrupt. 
    sei();
}

int main()
{	
	uint8_t r,c;
	uint8_t i;
	
	init_UART();
	timer1();
	
	while (1);
} 



