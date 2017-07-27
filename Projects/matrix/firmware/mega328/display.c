/*
Game of Life charliplexing 4 8x8 LED Matrix demo.
Designed for AtMega

_______________________________________________________________________________
# to compile and upload using avr-gcc and avrdude:

# compile
avr-gcc -Os -mmcu=atmega328p ./display.c -o a.o
# grab the text and data sections and pack them into a binary
avr-objcopy -j .text -j .data -O binary a.o a.bin 
# check that the binary is small enough to fit!
du -b ./a.bin
# upload using the avr ISP MKII. In this case, 
# it is located at /dev/ttyUSB1, but you would change that argument
# to reflect whichever device your programmer has been mounted as
avrdude -c avrispmkII -p m328p -B20 -P /dev/ttyUSB1 -U flash:w:a.bin

Hardware notes
===============================================================================

DDRx  : 1 = output, 0 = input
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
    PIN  : digital input

thanks to http://brownsofa.org/blog/archives/215 for explaining timer interrupts
*/

// avr io gives us common pin definitions, 
// and avr interrupt gives us definitions for setting interrupts. 
// ( we will use one timer interrupt to update the display )
#include <avr/io.h>
#include <avr/interrupt.h>

// Basic definitions. Our display is 16 pixels wide and 16 pixels high
// This is set in N
// There are 16*16=256 lights total, this is set in NN
// We also use NOP to control timing sometimes, we use an assembly wrapper
// reflected in the macro "NOP"
#define N 16
#define NN ((N)*(N))
#define NOP __asm__("nop\n\t")

// I use two different sets of buffers for display data. Part of this is 
// Laziness -- it does consume a lot of memory and wouldn't fly on say an
// AtMega8. But it makes the code a little easier to understand. 
// First, I store the display pixels in a "raster-life" format. Here, 
// Each rown of the display is stored in a sequence of integers, and 
// If a pixel is on, then the bit corresponding to that pixel is set to 1,
// Otherwise 0.
// These displays are used to run the game of life logic because it is 
// straightforward to read and write pixel data. 
// We use 16 bit integers because this makes more sense for a 16x16 array
// But this is an abstraction and 8 or 32 bit integers would work just as well.
// We use a double buffering strategy. So, we declare two buffers, and then
// also make two indirect pointers to these buffers. When we want to update
// the display state, we write into the "buff" pointer, while reading the
// previous game state from the "disp" pointer. Then, when we are ready to
// show the next frame, these pointers will be flipped.
#define BUFFLEN 16
uint16_t b0[BUFFLEN];
uint16_t b1[BUFFLEN];
uint16_t *buff=&b0[0];
uint16_t *disp=&b1[0];

// To actually scan the display, I use a sparse format. When we scan the lights
// we turn them on one at a time. This can make the display very dim if we
// have to turn on all of the lights. However, for something like the Game of
// Life, only a few lights are ever on at the same time. If we only worry 
// about scanning the lights that are on, then our display is much brighter.
// However, it is important that the code that scans the display is very
// fast, so that we can run it thousands of times per second so that there is
// no visible flicker to the human eye. So, we prepare a list of which lights
// are on ahead of time. Then, the display scanning code only has to refer to
// this list, which is rapid.
// We use a double buffering strategy here. The active list of lights that 
// are on is stored in the "lightList" variable. This is the one that the 
// display scanning code acually uses. When we want to prepare a new list,
// we write it into the lightBuff variable, then flip it once it is ready. 
// I am somewhat wasteful here and allocate enough space to store two lists
// that might contain all 256 LEDs. I suspect there are ways to use less 
// space by being clever, but I can't think of anything at the moment. 
uint8_t ll0[NN],ll1[NN];
volatile uint8_t *lightList = &ll0[0];
volatile uint8_t *lightBuff = &ll1[0];
volatile uint16_t lighted = 0;

// This is a simple random number generator. It is not very good, and will
// produce the same sequence of numbers each time the AtMega is turned on,
// but it will siffice for this demonstration.
uint32_t rng = 6719;
uint8_t rintn()
{
    rng = ((uint64_t)rng * 279470273UL) % 4294967291UL;
    return rng&0xff;
}

// To simplify changing the pin states of the AtMega, I group the three ports
// PORTB PORTC and PORTD into one logical port and then set all three with
// one function call. 
void setDDR(uint32_t ddr) 
{
    DDRB = ddr & 0xff;
    ddr >>= 8;
    DDRD = ddr & 0xff;
    ddr >>= 8;
    DDRC = ddr & 0xff;
}
void setPort(uint32_t pins)
{
    PORTB = pins & 0xff;
    pins >>= 8;
    PORTD = pins & 0xff;
    pins >>= 8;
    PORTC = pins & 0xff;
}

// Spin around NOP for a while -- a quick and lazy way to control timing
void delay(uint32_t n)  { while (n--) NOP;}

// Read and write functions for the display data. 
// This abstracts away the bit packing and unpacking.
// First argument: one of the display buffers ( either disp or buff )
// Second argument: the pixel index. We use row-major indexing, so 
// for example, the first row will count up indecies 0..15, then 
// the second row will start at index 16 .. 31 and so on and so forth.
// For the set method, the third argument is 0 or 1 -- 1 for "on" and 0 for
// "off"
uint8_t get(uint16_t *b,uint16_t i) { return (b[i>>4]>>(i&15))&1;}
uint8_t set(uint16_t *b,uint16_t i,uint8_t v) { if (get(b,i)!=v) b[i>>4]^=1<<(i&15);}

// The game of life wraps around at the edges. To abstract away some of the
// difficulty, these previous and next functions will return the previous
// or next row or column, automatically handlign wrapping around the edges.
// For example, the next column after column 15 is column 0. The previous
// row to row 0 is row 15.
uint8_t prev(uint8_t x) { return (x>0?x:N)-1; }
uint8_t next(uint8_t x) { return x<N-1?x+1:0; }

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

// These are some macros to make coding the Game of Life a little easier. 
// When we update the game, we are always reading from the current state, 
// which is stored in the "disp" display buffer, and we are always 
// writing the next state into the "buff" display buffer. So, we can 
// simplify the Get and Set functions for readability.
#define getLifeRaster(i)    get(disp,i)
#define setLifeRaster(i,v)  set(buff,i,(v))
// We store the display data in a bit-vector which is row-major ordered. 
// The bit vector is indexed by a single number from 0 to 255, but we want 
// to think about the Game of Life in terms of rows and columns. So, 
// this macro just wraps conversion of row and column numbers into an index
// for readability
#define rc2i(r,c)           ((r)*N+(c))
// Finally, combine the index macro with the get and set macros to 
// create macros for reading and writing game state based on the row and
// column numbers. 
#define getLife(r,c)        getLifeRaster(rc2i(r,c))
#define setLife(r,c,v)      setLifeRaster(rc2i(r,c),v)

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
uint8_t columnCount(r,c) {
	return getLife(prev(r),c) + getLife(r,c) + getLife(next(r),c);
}

// We store the game state in raster-like buffers of pixel data, but we
// scan the display one light at a time based on a list of which lights are
// on. This function converts from the raster-like representation to the
// list reprentation. Once we have prepared a new frame of the game, we call
// this function to create a new list of "on" lights for the display scanning
// code. We also flip the pixel and light-list buffers here. 
void flipBuffers() 
{
	// flip the pixel buffers
    uint16_t *temp = buff;
    buff = disp;
    disp = temp;
    
    // scan the new pixel buffer for lit pixels and add them to the list
    // of lighted pixels for sparse scanning
    uint16_t r,c,i;
    uint16_t lightCount = 0;
    for (i=0;i<NN;i++)
    {
        if (get(disp,i))
        {
            lightBuff[lightCount]=i;
            lightList[lightCount]=i;
            lightCount++;
        }
    }
    
    //swap the sparse scanning data buffers
    lighted = lighted<lightCount?lighted:lightCount;
    
    volatile uint8_t *ltemp = lightBuff;
    lightBuff = lightList;
    lightList = ltemp;
    
    lighted = lightCount;
}

// pin definitions. We have wired up four different 8x8 LED matrices to
// The AtMega. I have kept the anodes and cathodes contiguous, so that
// For each array, the anodes and the cathodes use a sequential number of 
// Pins. The way I have set up my logical pin numbers, Pins 0..7 are PORTB
// Pins 8..15 are PORTD, and then the rest are PORTC. These arrays store
// The anode or cathode sequence starts for each of the 4 matrices.
const uint8_t cmap[4]  = { 21,9, 7,17};
const uint8_t amap[4]  = { 12,1,15, 4};

// I didn't know which anode/cathode was which when I wired up the arrays.
// LED matrix pinouts can be idiosyncratic like that. However, I did wire up
// each of the arrays in a consistent manner. So, I can fix this by applying
// a permutation in software. These arrays store the pin permuations for 
// the anodes and cathodes.
const uint8_t aperm[N] = {0,2,3,1,7,4,6,5};
const uint8_t cperm[N] = {4,3,1,5,0,6,7,2};

// This function handles the display scanning -- it is called from a timer
// interrupt. The variable scanI keeps track of which light we're currently
// displaying. The variable PWM keeps track of state so that we can turn some
// lights on longer than others. Each time a timer interrupt occurs, this
// code changes which light is displayed. Lights are only shown one at a time.
// The timer interrupt changes them fast enough that they appear to be all
// Illuminated simultaneously.
volatile uint8_t scanI = 0;
volatile uint8_t pwm   = 0;
ISR(TIMER0_COMPA_vect) 
{
	// PWM would allow us to vary the brightness by leaving some lights
	// on longer. Here, I just use it to correct for a problem with PORTC.
	// For some reason LEDs on PORTC are more dim than they should be. This 
	// is probably because I am driving the LEDs without current limiting 
	// resistors, and so LEDs on other ports are drawing more current than
	// I expected ( more than 40mA ). Thus, these LEDs are brighter than the
	// ones driven using PORTC. It may have something to do with PORTC also
	// being capable of analog input? Anyway, its unclear if this is bad but
	// this little PWM script seems to fix it. Also, I intentionally waste
	// CPU cycles using a delay loop so that the PWM branch takes as long
	// as the light-scanning branch -- this is so that the fraction of time
	// taken by the display scanning interrupt routine is roughtly constant.
	// This is important because if I returned early from the PWM branch, 
	// The extra CPU cycles would be used by the game logic and the game
	// frame rate would vary. A more correct way to do this is to wait after
	// each frame of the game of life has been computer, based on timer
	// interrupts. But just adding the delay here was easier for me.
	if (pwm)
	{
		pwm--;
		delay(110);
		return;
	}
	
	// We advance to the next light. If for some reason all of the lights are
	// off we return immediately. Technically, there are race conditions
	// with the flipBuffers() function, but the errors are not catestrophic
	// We only focus on lights that are on, and keep track of how many lights
	// are on in the "ligted" variable. 
	scanI++;
	if (!lighted) return;
	scanI%=lighted;
	
	// We look up which light should be on, which is stored in the lightList.
	// The lower 4 bits contain the column index, and the upper four bits 
	// contain the row index. Our display consists of 4 8x8 arrays. These are
	// laid out with a charlieplexing pattern. So, we do a test to see which
	// array our light at row, columb (r,c) should be in -- that is what the
	// variable 'b' stored. Then we can lookup which anode and cathode we 
	// should use to turn the light on, now that we know which array to use.
	uint8_t light = lightList[scanI];
	uint8_t r = light>>4;
	uint8_t c = light&0x0f;
	uint8_t b = (r<8?0:1)+(c<8?0:2);
	
	// Looking up which anode and cathode to use seems strange at first.
	// For each array, I've wired up the anodes and cathodes to be contiguous
	// So that, for example, the fourth array, the cathodes will start at 
	// logical pin 17, and proceed to logical pin 25. Except there are only
	// 21 pins, so it wraps around to the beginnign again rather than counting
	// Up to 24. So, we get the starting in, an count up from there, and use
	// Modulo to wrap around to the beginning again. 
	// Finally, I did not know which cathode or anode was which when I wired
	// up the arrays -- LED matrix pinouts can be rather idiosyncratic like 
	// that. So! The anodes and cathods were all out of order. So I have to
	// look up a permutation to get the right one for our specific row and
	// column. 
	uint8_t an = (amap[b]+aperm[r%8]+21)%22;
	uint8_t ct = (cmap[b]+cperm[c%8]+21)%22;

	// Now that we know which pins to use to turn on the light, we can 
	// actually set the pin state to achieve this. First we turn all the 
	// pins to high impedence ("Off" or "input mode"). This is because, to
	// set the right pin, we have to changes pins in PORTA, PORTB, and PORTC.
	// As far as I know, there is no way to change all three of these 
	// simultanously -- you have to do them one at a time. This means that
	// in the process of changing the pin states we might accidentally turn on
	// some lights we didn't mean to. To work around this, first turn off all
	// the pins, then set the pin state, then turn back on only the pins that
	// we need to use
	setDDR(0);
	setPort(1UL<<an);
	setDDR((1UL<<an)|(1UL<<ct));
	
	// If we are driving the anodes using PORTC, we use a brightness correction
	if (an>=16) pwm = 1;
}

int main()
{
	// I happen to be using the internal RC oscillator, which has an unknown
	// frequency. Setting OSCCAL to 0xff means "run the internal RC oscillator
	// "FAST". OSCCAL is supposed to be used for calirating the RC oscillator 
	// so that it runs at a known frequency, but here I just max out the 
	// calibration variable. Another way to do this is to change the fuses so 
	// that 	// the RC oscillator runs faster -- but this works too. 
	OSCCAL=0xFF;

	// I want to configure a timer interrupt to chan the display.
	// I determined these values using the AtMega datasheet, and don't exactly
	// remember what they all mean. 
	// I know that we turn on timer interrupt 0 and set the prescaler 
	// (in TCCR0B) to something -- probably "as fast as you can". 
	// Then, we set it to "compare to counter" mode. This means that the
	// counter will incremement, and when it gets to OCR0A ( set here to 180 )
	// it will call our display scanning code. This lets you tweak how often
	// the display is scanned. If it too slow, you will see flickering. If
	// it is too fast, there will not be enough CPU cycles left over to run
	// the game logic.
    TIMSK0 = 2;  // Timer CompA interupt 1
    TCCR0B = 2;  // speed
    TCCR0A = 2;  // CTC mode
    OCR0A  = 180;// period
    
    // Initialize the game to a random state, and update the lightList to
    // reflect this.
    uint8_t r,c;
    for (r=0;r<16;r++) buff[r] = rintn();
    flipBuffers();
    sei();
    
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
                uint8_t cell = getLife(r,c);
                uint8_t upcoming = columnCount(r,next(c));
                neighbor += upcoming;
                uint8_t new = cell? (neighbor+1>>1)==2:neighbor==3;
                neighbor -= previous;
                previous = current ;
                current  = upcoming;
                changed |= new!=cell && new!=get(buff,rc2i(r,c));
                k += new;
                setLife(r,c,new);
            }
        }

        uint8_t l=0;
        if (!(k&&rintn())) l=genesis;
        if (!changed && shownFor++>TIMEOUT) l=genesis;
        if (l) {
            uint8_t r = rintn();
            uint8_t q = rintn();
            uint8_t a = rintn()&1;
            uint8_t b = rintn()&1;
            uint8_t i,j;
            for (i=0;i<3;i++)
            {    
                uint8_t c = q;
                for (j=0;j<3;j++) 
                {
                    setLife(r,c,(l&0x80)>>7);
                    l <<= 1;
                    c = a?next(c):prev(c);
                }
                r = b?next(r):prev(r);
            }    
            shownFor = 0;
        }
        flipBuffers();
    }
} 

