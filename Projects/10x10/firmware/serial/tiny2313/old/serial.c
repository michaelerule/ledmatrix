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
avrdude -c avrispmkII -p m328p -B100 -P /dev/ttyUSB0 -U lfuse:w:0xE2:m 

run this to change it back :
avrdude -c avrispmkII -p m328p -B100 -P /dev/ttyUSB0 -U lfuse:w:0x62:m 

to check serial,
-- set up to listen
-- listen in screen
-- set firmware to scan baudrate
-- wait for something sane to appear in screen
-- exit
-- use this baudrate

stty -F /dev/ttyUSB1 9600 cs8 cread
screen /dev/ttyUSB1
C-a : quit

============================================================================
*/
#include <avr/io.h>
#include <avr/interrupt.h>

void InitUART (uint8_t baudrate) {
    UBRRL = baudrate;                    // Set the baud rate 
    UCSRB = (1 << RXEN ) | (1 << TXEN ); // Enable UART receive transmit 
    UCSRC = (1 << UCSZ1) | (1 << UCSZ0); // 8 data bits, 1 stop bit 
}

void Transmit(uint8_t data) {
    while (!(UCSRA & (1 << UDRE))); // Wait clear transmit buffer
    UDR = data;
}

// baud rate scanner for the lazy
void baudscan() {
    uint8_t i;
    uint16_t br = 1;
    while(1) {
        InitUART(br);
        uint16_t br2 = br;
        uint16_t i;
        for (i=0; i<100; i++)
            Transmit('\n'); 
        for (i=0; i<100; i++)
            Transmit('!'); 
        for (i=0; i<100; i++)
        {
            br2 = br;
            Transmit('\n'); 
            while (br2)
            {
                uint8_t digit = br2%10;
                br2/=10;
                Transmit(48+digit);
            }
        }
        for (i=0; i<100; i++)
            Transmit('!'); 
        br ++;
    }
}

// just send stuff
void signal()
{
    uint8_t send=0;
    InitUART(25);
    while (1) Transmit(send++); 
}

// send back what you hear
void echo()
{
    uint8_t i;
    InitUART(26); // Set the baudrate to 2400 bps using a 8MHz crystal 
    uint8_t read='q';
    while (1) {
        if (UCSRA&(1<<RXC))
        {
            read = UDR;
            Transmit(read); 
        }
    }
}

// recieves a character then transmits back the next character
int main (void)
{
    //baudscan();
    //signal();
    echo();
}

