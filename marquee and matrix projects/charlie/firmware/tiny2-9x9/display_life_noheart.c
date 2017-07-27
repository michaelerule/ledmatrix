/*
Hardware notes
===============================================================================

PB1 and PB2 are now used for software serial!

	DDRx   : 1 = output, 0 = input
	PORTx : output buffer
	PINx  : digital input buffer ( writes set pullups )
	Port B is the chip IO port ( other ports are control registers )

		AtTiny24 pinout:
		            __ __
		3     VCC -|  U  |- GND    1
	(RX)  BCK PB0 -|     |- PA0 G1
		  SHU PB1 -|     |- PA1 B1
		4 RST PB3 -|     |- PA2 R1
		  PLY PB2 -|     |- PA3 G2 
		  FWD PA7 -|     |- PA4 B2 5
		2 IND PA6 -|_____|- PA5 R2 6
		        
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

April Karen Emma Nina

=============================================================================*/

#include <avr/io.h>
#define NOP __asm__("nop\n\t")
#include <avr/interrupt.h>

#define N 9
#define NN ((N)*(N))
#define SCANRATE 100

uint16_t b0[N+1],b1[N+1];
uint16_t *disp = &b0[0];
uint16_t *buff = &b1[0];

void delay(uint32_t n)  { while (n--) NOP;}

const uint8_t pinPermutation[N+1] = {8,9,7,6,0,1,2,3,4,5};

/*
Due to memory and speed constraints we store the display data in a format
representing a series of pin-states needed to scan the display. This 
avoids extra computation in the display scanning interrupt and also is a
very compact format that fits in a small amount of memory.

In charlieplexing, we use the data direction registers to set which pixels
are on, using the "high impedence" mode for the "off" state. 
So, a data-direction register state of 0xffff corresponds to "all lights on"

The diagonal r==c doesn't work because you'd be driving the anode and cathode
from the same pin, so we use the last (row 9) to drive the se ones.

*/

/*
void scanDisplay(uint8_t scanI)
{
	uint16_t pin = ((uint16_t)1)<<scanI;
	uint16_t ddr = disp[scanI];
	DDRA  = 0  ; DDRB  = 0;
	PORTA = pin; PORTB = pin>>7;
	DDRA  = ddr; DDRB  = ddr>>7;
}
*/

volatile uint8_t scanI = 0;
ISR(TIM0_COMPA_vect) {
	uint16_t pin = ((uint16_t)1)<<scanI;
	uint16_t ddr = disp[scanI];
	DDRA  = 0  ; DDRB  = 0;
	PORTA = pin; PORTB = pin>>7;
	DDRA  = ddr; DDRB  = ddr>>7;
	if (++scanI>(1+N)) scanI=0;
}

void setup_display() {
	TIMSK0 = 2;        // Timer CompA interupt
	TCCR0B = 2;        // speed
	TCCR0A = 2;        // CTC mode
	OCR0A  = SCANRATE; // period
	sei();
}

void set(uint16_t *buffer,uint8_t r,uint8_t c,uint8_t value)
{
	while (r>N) r-=N;
	while (c>N) c-=N;
	uint8_t row,col;
	if (r==c)
		c=N;
	row=pinPermutation[r];
	col=pinPermutation[c];
	if  (!value) buffer[col]&=~(1UL<<row);
	else         buffer[col]|= (1UL<<row);   
}

uint8_t get(uint16_t *buffer,uint8_t r,uint8_t c)
{
	while (r>N) r-=N;
	while (c>N) c-=N;
	uint8_t row,col;
	if (r==c) c=N;
	row=pinPermutation[r];
	col=pinPermutation[c];
	return ((buffer[col]>>row)&1);
}

void clearDisplay(uint16_t *buffer)
{
	uint8_t i,r,c;
	for (i=0;i<N+1;i++) buffer[i]=0xffff; //clear display memory
	for (r=0;r<N;r++) for (c=0;c<N;c++) set(buffer,r,c,0);
}

// The game of life wraps around at the edges. To abstract away some of the
// difficulty, these previous and next functions will return the previous
// or next row or column, automatically handlign wrapping around the edges.
// For example, the next column after column 15 is column 0. The previous
// row to row 0 is row 15.
uint8_t prev(uint8_t x) { 
	if (x==0) return N-1;
	if (x<N) return x-1;
	return 0;
}
uint8_t next(uint8_t x)  { 
	if (x<(N-1)) return x+1;
	if (x==(N-1)) return 0;
	return 0;
}
// These are parameters to configure the Game of Life. Sometimes the game
// gets stuck. To poke it, I randomly drop in some primative shapes. 
// I have stored these shapes in a bit-packed representation here.
// Also, the TIMEOUT variable tells us how long to let the game stay "stuck"
// before we try to add a new life-form to it. The variable shownFor counts
// how many frames the game has been stuck.
#define glider  0xCE
#define genesis 0x5E
#define bomb    0x5D
#define TIMEOUT 7
uint8_t shownFor;

// One step in the Game of Life is to count the number of neighbors that 
// are "on". This function computes part of that: it counts the number of 
// neighbors that are "on" at the current position (r,c) and also the
// row above and below. This is not a Macro because the Game of Life 
// implementation here was ported from an implementation with much less
// flash memory. When you make a macro, that code is expanded with simple
// text substitution before it even gets to the compiler. This means that
// each time we "call" a macro a bunch of code gets included. Somtimes this
// can be optimized out, but not always. By making this a function, we help
// the compiler understand that each "call" can be computed using the same
// code, and this resulted in a smaller binary. TLDR: esoteric design choice
// for size optimization, IGNORE.
inline uint8_t columnCount(r,c) {
    return get(disp,prev(r),c) + get(disp,r,c) + get(disp,next(r),c);
}

/*
	read temperature
	-- set ADC to use internal voltage reference
	-- set ADC to read temp sensor
	-- set ADC to max resolution
	-- turn on ADC
	-- wait a few cycles
	-- clear conversion bit
	-- initiate conversion
	-- wait for converstion to complete
	-- read result low bit
	-- read read result high bit
*/
uint16_t temperature()
{
	ADMUX  = 0b10100010; // set up for temp read with internal 1.1 vref
	ADCSRA |= 1<<7; // turn on ADC
	ADCSRA |= 1<<6; // initiate a conversion
	while ((ADCSRA>>6)&1); // wait for conversion to finish
	uint16_t data;
	data = ADCL;
	data |= ((uint16_t)ADCH)<<8;
	return data &0b1111111111;
}

uint16_t voltage()
{
	ADMUX = 0b01100001; // read the internal 1.1v ref against VCC
	ADCSRA |= 1<<7; // turn on ADC
	ADCSRA |= 1<<6; // initiate a conversion
	while ((ADCSRA>>6)&1); // wait for conversion to finish
	uint16_t data;
	data = ADCL;
	data |= ((uint16_t)ADCH)<<8;
	ADMUX = 0;
	return data &0b1111111111;
}

uint16_t analogRead(uint8_t pin)
{
	ADMUX = 0b10000000|pin; // read the pin ref against VCC
	ADCSRA |= 1<<7; // turn on ADC if not already on
	ADCSRA |= 1<<6; // initiate a conversion
	while ((ADCSRA>>6)&1); // wait for conversion to finish
	uint16_t data;
	data = ADCL;
	data |= ((uint16_t)ADCH)<<8;
	ADMUX = 0;
	return data &0b1111111111;
}

uint8_t entropy()
{
	uint8_t x=0,i;
	for (i=0;i<8;i++) 
	{
		uint16_t t=temperature(i);
		t ^= t>>8;
		t ^= t>>4;
		t ^= t>>2;
		t ^= t>>1;
		x|=(t&1)<<i;
	}
	ADCSRA=0;
	return x;
}

void heart(uint16_t *buffer)
{
	set(buffer,7,4,1);
	set(buffer,6,3,1);
	set(buffer,6,5,1);
	set(buffer,5,2,1);
	set(buffer,5,6,1);
	set(buffer,4,1,1);
	set(buffer,4,7,1);
	set(buffer,3,1,1);
	set(buffer,3,7,1);
	set(buffer,2,2,1);
	set(buffer,2,6,1);
	set(buffer,2,3,1);
	set(buffer,2,5,1);
	set(buffer,3,4,1);
}

int main() {	
    OSCCAL=10;
	uint8_t i,j,r,c;
	
	// TEST: LIGHT ALL THE LIGHTS
	/*
	while(1) for (c=0;c<10;c++) for (r=0;r<10;r++) 
	{
		uint16_t pin = ((uint16_t)1)<<c;
		uint16_t ddr = pin | (((uint16_t)1)<<r);
		DDRA  = 0  ; DDRB  = 0;
		PORTA = pin; PORTB = pin>>7&0b1110;
		DDRA  = ddr; DDRB  = ddr>>7&0b1110;
	}
	*/
	
	//TEST: scan columns
	//Column permutation:
	// 56789 (diag) 4312
	// 01234 5      6789
	//Inverse permutation
	// 897601234 (5)
	/*
	while(1) for (c=0;c<10;c++) for (i=0;i<255;i++) for (j=0;j<5;j++) for (r=0;r<10;r++) 
	{
		uint16_t pin = ((uint16_t)1)<<c;
		uint16_t ddr = pin | (((uint16_t)1)<<r);
		DDRA  = 0  ; DDRB  = 0;
		PORTA = pin; PORTB = pin>>7&0b1110;
		DDRA  = ddr; DDRB  = ddr>>7&0b1110;
	}
	*/
	
	//TEST: scan rows
	//Row permutation:
	// 56789 (none) 4312
	// 01234 5      6789
	//Inverse permutation
	// 897601234 (5)
	/*
	while(1) for (r=0;r<10;r++) for (i=0;i<255;i++) for (j=0;j<5;j++) for (c=0;c<10;c++)
	{
		uint16_t pin = ((uint16_t)1)<<c;
		uint16_t ddr = pin | (((uint16_t)1)<<r);
		DDRA  = 0  ; DDRB  = 0;
		PORTA = pin; PORTB = pin>>7&0b1110;
		DDRA  = ddr; DDRB  = ddr>>7&0b1110;
	}
	*/
	
	//TEST: in order row column scan
	/*
	while(1) for (r=0;r<9;r++) for (c=0;c<9;c++) for (i=0;i<255;i++)
	{
		uint16_t pin = ((uint16_t)1)<<pinPermutation[r==c?9:c];
		uint16_t ddr = pin | (((uint16_t)1)<<pinPermutation[r]);
		DDRA  = 0  ; DDRB  = 0;
		PORTA = pin; PORTB = pin>>7&0b1110;
		DDRA  = ddr; DDRB  = ddr>>7&0b1110;
	}
	*/
	
	//TEST: N+1 word in order scan
	/*while(1) for (r=0;r<10;r++) for (i=0;i<255;i++) for (j=0;j<255;j++)
	{
		uint16_t pin = ((uint16_t)1)<<r;
		uint16_t ddr = 0xffff;
		DDRA  = 0  ; DDRB  = 0;
		PORTA = pin; PORTB = pin>>7&0b1110;
		DDRA  = ddr; DDRB  = ddr>>7&0b1110;
	}*/
	
	//TEST: display memory ( manual set )
	/*
	for (c=0;c<9;c++) for (r=0;r<9;r++) 
	{
		uint8_t row,col;
		if (r==c)
			col=5;
		else
			col=pinPermutation[c];
		row=pinPermutation[r];
		if ((c^r)&1) disp[col]|= (1UL<<row);
		else         disp[col]&=~(1UL<<row);
	}
	while(1) for (r=0;r<10;r++)
	{
		uint16_t pin = ((uint16_t)1)<<r;
		uint16_t ddr = ~disp[r];
		DDRA  = 0  ; DDRB  = 0;
		PORTA = pin; PORTB = pin>>7&0b1110;
		DDRA  = ddr; DDRB  = ddr>>7&0b1110;
	}
	*/
	
	//TEST: display memory with set
	/*
	for (c=0;c<9;c++) for (r=0;r<9;r++) 
		set(disp,r,c,(c^r)&1);
	while(1) for (r=0;r<10;r++)
	{
		uint16_t pin = ((uint16_t)1)<<r;
		uint16_t ddr = ~disp[r];
		DDRA  = 0  ; DDRB  = 0;
		PORTA = pin; PORTB = pin>>7&0b1110;
		DDRA  = ddr; DDRB  = ddr>>7&0b1110;
	}
	*/
	
	//TEST: display memory with set and scanDisplay
	/*
	for (c=0;c<9;c++) for (r=0;r<9;r++) 
		set(disp,r,c,(c^r)&1);
	while(1) for (r=0;r<10;r++) scanDisplay(r);
	*/
	
	//TEST: display memory with set and scanDisplay with interrupts
	/*
	for (c=0;c<9;c++) for (r=0;r<9;r++)  set(disp,r,c,(c^r)&1);
	for (c=0;c<9;c++) for (r=0;r<9;r++)  set(disp,r,c,c&r&1);
	setup_display();
	while(1);
	*/
	
	//TEST: get
	/*
	for (c=0;c<9;c++) for (r=0;r<9;r++) set(buff,r,c,(c^r)&1);
	for (c=0;c<9;c++) for (r=0;r<9;r++) set(buff,r,c,c&1);
	for (c=0;c<9;c++) for (r=0;r<9;r++) set(buff,r,c,r&1);
	for (c=0;c<9;c++) for (r=0;r<9;r++) set(buff,r,c,r&c&1);
	for (c=0;c<9;c++) for (r=0;r<9;r++) set(buff,r,c,(r^c)&2);
	for (c=0;c<9;c++) for (r=0;r<9;r++) set(disp,r,c,get(buff,r,c));
	setup_display();
	while(1);
	*/
	
	//TEST: gather entropy
	// -- internal temperature
	// -- supply voltage
	/*
	 The voltage reference for the ADC may be selected by writing to the REFS1:0 bits in ADMUX
	 we want to use the supply voltage for measuring the reference band gap and temperature
	 the reference band gap may be better for getting entropy from analog in since it is smaller and
	 so might blow up noise more
	 The analog input channel and differential gain are selected by writing to the MUX5:0 bits in ADMUX
	 The on-chip temperature sensor is selected by writing the code “100010” to the MUX5:0 bits in ADMUX register.
	 The ADC is enabled by setting the ADC Enable bit, ADEN in ADCSRA.
	 The ADC does not consume power when ADEN is cleared
	 The ADC generates a 10-bit result which is presented in the ADC Data Registers, ADCH and 
	 Once ADCL is read, ADC access to data registers is blocked. This means that if ADCL has been read, 
	 and a conversion completes before ADCH is read, neither register is updated and the result 
	 from the conversion is lost. When ADCH is read, ADC access to the ADCH and ADCL Registers is re-enabledADCL
	 The ADC has its own interrupt which can be triggered when a conversion completes. When ADC
	 access to the data registers is prohibited between reading of ADCH and ADCL, the interrupt will
	 trigger even if the result is lost.
	 A single conversion is started by writing a logical one to the ADC Start Conversion bit, ADSC
	*/
	
	/*
	read supply voltage
	
	*/
	//ADMUX = 0b01100001; // set up to read 1.1vref against supply voltage

	uint16_t *temp;
	clearDisplay(disp);
	clearDisplay(buff);
	for (r=0;r<N;r++) for (c=0;c<N;c++) set(disp,r,c,1);
	setup_display();
	
	//while(1)
	//delay(700000);
	
	//GAME OF LIFE
	cli();
	clearDisplay(disp);
	for (r=0;r<N;r++) for (c=0;c<N;c++) set(disp,r,c,entropy()&1);
	sei();
	

	//while(1) entropy();
    // Run the Game of Life
    // There are some tedious optimizations here. 
    // To compute the next frame in the Game of Life, one has to sum up
    // the numbr of "live" or "on" neighbors around each cell.
    // To optimize this, we keep a running count. 
    // When we want to count the number of "live" cells at position (r,c),
    // We look at how many cells were alive around position (r,c-1). Then, we 
    // Subtract the cells from column c-2 and add the cells from column c+1.
    // One could also keep a running count along the columns, but this
    // would require more intermediate state and the additional memory lookups
    // consume the benefit in this application. 
    // As we update the game, we also keep track of whetehr a cell changes. 
    // If cells are not changing, we make note of this, and if the game stays
    // stuck for a bit, then we add in new life-forms to keep things
    // interesting. 
    // One neat thing about double-buffering the game state is that the 
    // state from TWO frames ago is available in the output buffer, at least,
    // until we write the next frame over it. So we can take advantage of this
    // and also detect when the game gets stuck in a 2-cycle.
    while (1) 
    {
        uint8_t changed = 0;
        uint8_t k=0;
        for (r=0; r<N; r++)
        {
            uint8_t previous = columnCount(r,N-1);
            uint8_t current  = columnCount(r,0);
            uint8_t neighbor = previous+current;
            for (c=0; c<N; c++)
            {
                uint8_t cell = get(disp,r,c);

                uint8_t upcoming = columnCount(r,next(c));
                neighbor += upcoming;
                uint8_t new = cell? (neighbor+1>>1)==2:neighbor==3;

                neighbor-= previous;
                previous = current ;
                current  = upcoming;
                changed |= new!=cell && new!=get(buff,r,c);
                k += new;
                set(buff,r,c,new);
            }
        }
		
        uint8_t l=0;
        if (!(k&&entropy())) l=genesis;
        if (!((entropy()|entropy()|entropy()|entropy()|entropy()|entropy())&1) ) l=bomb;
        if (!changed && shownFor++>TIMEOUT) l=genesis;
        if (l) {
            uint8_t r = entropy()%N;
            uint8_t q = entropy()%N;
            uint8_t a = entropy()&1;
            uint8_t b = entropy()&1;
            uint8_t i,j;
            for (i=0;i<3;i++)
            {    
                uint8_t c = q;
                for (j=0;j<3;j++) 
                {
                    set(buff,r,c,(l&0x80)>>7?1:0);
                    l <<= 1;
                    c = a?next(c):prev(c);
                }
                r = b?next(r):prev(r);
            }    
            shownFor = 0;
        }
        
        // flip the pixel buffers
		temp = buff; buff = disp; disp = temp;
    }
    
} 



