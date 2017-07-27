#include <avr/io.h>
#include <stdint.h> 
#include <arduino.h>
#include <interrupt.h>
#include <WProgram.h>
#include <Arduino.h>

#define REFRESH_PRESCALER 2 // The prescaler divides the system clock to determine the display scanning clock ( 2 works, 4 is slower )
#define REFRESH_INTERVAL  37 // The interval determines how many prescaled clock ticks to wait before scanning the display

#define BITDEPTH 5 // BITDEPTH sets the number of color bits to use in the internal display representation
#define NCOLOR   10 // NCOLOR sets the number of actual colors to use -- mapped to bit representation by the GAMMA_CORRECTED variable
const uint8_t GAMMA_CORRECTED[NCOLOR] = {0,1,2,3,5,7,9,15,20,31};

#define LATCHPIN 2      //Pin connected to data latch clock of 74HC595 on PORTD ( rise triggered )
#define CLOCKPIN 3      //Pin connected to data line  clock of 74HC595 on PORTD ( rise triggered )
#define LATCHPORT PORTD //Port containing the serial latch line
#define CLOCKPORT PORTD //Port containing the serial clock line
#define LATCHMASK _BV(LATCHPIN)
#define CLOCKMASK _BV(CLOCKPIN)
#define CLOCKTICK { CLOCKPORT = 0; CLOCKPORT = CLOCKMASK; }
#define LATCHTICK { LATCHPORT = 0; LATCHPORT = LATCHMASK; }

#define DATAPORT PORTB //Port contaning  the parallel-serial data lines
#define NDATALINES 6
const uint8_t dataLines[NDATALINES] = {0,1,2,3,4,5};

#define PORTCOLUMNMASK   0b00000111
#define PORTROWMASK      0b00111000
#define PORTCOLBITSSTART 0
#define PORTROWBITSSTART 3
#define NCOLREGISTERS    3
#define NROWREGISTERS    3

#define display_dtype uint8_t
#define BYTESPERCOLUMN    3
#define NFRAMES           BITDEPTH
#define BYTESPERFRAME    (NCOLS*BYTESPERCOLUMN)
#define DISPLAYBUFFERLEN (BYTESPERFRAME*NFRAMES)

display_dtype __db1[DISPLAYBUFFERLEN*2];
display_dtype *drawing_buffer = &__db1[0];
display_dtype *display_buffer = &__db1[DISPLAYBUFFERLEN];

volatile uint16_t display_scan_index = 0;

inline void blankDisplay()  {
  DATAPORT = 0xff;
  for (int i=0; i<=8; i++) CLOCKTICK;
  LATCHTICK;
}

inline void lightDisplay()  {
  DATAPORT = 0x00;
  for (int i=0; i<=8; i++) CLOCKTICK;
  LATCHTICK;
}

inline void setBufferPixel(uint8_t r, uint8_t c, uint8_t v, display_dtype *buffer) {
  r = 19-r;
  uint8_t row_byte = r>>3;
  uint8_t row_bit  = r&0b111;
  buffer += c * BYTESPERCOLUMN;
  for (int f=0; f<NFRAMES; f++) { 
    if (v&1) buffer[row_byte] &= ~_BV(row_bit); // this assignemnet is reversed from normal to account for inverted signals
    else     buffer[row_byte] |=  _BV(row_bit);
    v >>= 1;
    buffer += BYTESPERFRAME;
  }
}

inline uint8_t getBufferPixel(int r, int c, display_dtype *buffer) {
  r = 19-r;
  uint8_t row_byte = r>>3;
  uint8_t row_bit  = r&0b111;
  buffer += c * BYTESPERCOLUMN;
  uint8_t v = 0;
  for (int f=0; f<NFRAMES; f++) { 
    v |= (((buffer[row_byte]>>row_bit)&1)^1)<<f;// The ^1 flips the bit accounting for the inverted signals
    buffer += BYTESPERFRAME;
  }
  return v;  
}

inline void setBufferColor(uint8_t r, uint8_t c, int8_t v, display_dtype *buffer) {
  setBufferPixel(r,c,GAMMA_CORRECTED[v>=NCOLOR?NCOLOR-1:v<0?0:v],buffer);
}

#define setPixel(r,c,v) setBufferPixel(r,c,v,drawing_buffer)
#define setColor(r,c,v) setBufferColor(r,c,v,drawing_buffer)
#define getPixel(r,c)   getBufferPixel(r,c,  display_buffer)

volatile uint8_t current_frame         = 0;
volatile uint8_t current_column        = 0;
volatile uint8_t current_frame_counter = 0;

/** Display driving interrupt routine.
 *  Data have been packed in 4 bytes per column chunks. 
 *  We need to write out both the column state and the 
 *  row state in a parallel-serial fashion to six shift
 *  registers over PORTB.
 *  The column data goes in the three least significant
 *  bits of PORTB. The column index goes in the next three. 
 *  To pack the column data, grab the Kth bit from all
 *  three column bytes. To pack the column index, create
 *  a byte sequence that is all 1s, except for a 0 at the
 *  current column. This will occupy three bytes. Index it
 *  as you would the column data. 
 */

uint8_t columbytes[8*NCOLS] = {
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,
    0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000,0b00111000};
 
ISR(TIMER0_COMPA_vect,ISR_NOBLOCK)
{
  //union colindex { uint32_t index; uint8_t bytes[4];} colindex;
  //colindex.index = ~(uint32_t)1<<current_column;
  //uint8_t state2 = colindex.bytes[2];
  //uint8_t state1 = colindex.bytes[1];
  //uint8_t state0 = colindex.bytes[0];
    
  display_dtype *buffer = display_buffer + current_frame*BYTESPERFRAME + current_column*BYTESPERCOLUMN;
  uint8_t state5 = buffer[2];
  uint8_t state4 = buffer[1];
  uint8_t state3 = buffer[0];
  /*
  for (int8_t i=7; i>=0; i--) {
    DATAPORT = (
      //(state0>>i&1)<<0 | (state1>>i&1)<<1 | (state2>>i&1)<<2 
      (state3>>i&1)<<3 | (state4>>i&1)<<4 | (state5>>i&1)<<5
      );    
    CLOCKTICK;
  }
  LATCHTICK;
  */
  DATAPORT = (state3>>4&0b00001000) | (state4>>3&0b00010000) | (state5>>2&0b00100000); CLOCKTICK;
  DATAPORT = (state3>>3&0b00001000) | (state4>>2&0b00010000) | (state5>>1&0b00100000); CLOCKTICK;
  DATAPORT = (state3>>2&0b00001000) | (state4>>1&0b00010000) | (state5>>0&0b00100000); CLOCKTICK;
  
  DATAPORT = (state3>>1&0b00001000) | (state4<<0&0b00010000) | (state5<<1&0b00100000); CLOCKTICK;
  state4<<=1;
  state5<<=2;
  DATAPORT =  (state3&0b00001000) | (state4&0b00010000) | (state5&0b00100000); CLOCKTICK;
  DATAPORT = ((state3&0b00000100) | (state4&0b00001000) | (state5&0b00010000)) <<1; CLOCKTICK;
  DATAPORT = ((state3&0b00000010) | (state4&0b00000100) | (state5&0b00001000)) <<2; CLOCKTICK;
  DATAPORT = ((state3&0b00000001) | (state4&0b00000010) | (state5&0b00000100)) <<3; CLOCKTICK;
  LATCHTICK;
  
  if (++current_column>=NCOLS) {
    current_column = 0;
    if (++current_frame_counter>=(1<<current_frame)) {
      current_frame_counter = 0;
      if (++current_frame>=BITDEPTH) current_frame = 0;      
    }
  }
}

void init_display()
{
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);
  for (int i=0; i<NDATALINES; i++) DDRB |= _BV(dataLines[i]);
  lightDisplay();
  for (uint16_t i=0; i<DISPLAYBUFFERLEN; i++) drawing_buffer[i]=0x00;
  for (uint16_t i=0; i<DISPLAYBUFFERLEN; i++) display_buffer[i]=0x00;
  
  // set up display interrupts
  TIMSK0 = 2; // Use timer 0 Compare interrupt A
  TCCR0A = 2; // Configure the compare interrupt for "compare to counter" mode ( resets counter on interrupt )
  TCCR0B = REFRESH_PRESCALER; 
  OCR0A  = REFRESH_INTERVAL; 
  sei();
}

//////////////////////////////////////////////////////////////////////////////////////

/** Flips the drawing and display buffers. 
 *  Contents of drawing buffer will be shown on screen
 *  Contents of screen will be moved to drawing buffer
 *  call clearDrawing() if you want to quickly start with a blank canvas
 *  after using this function */
void show() {
  display_dtype *temp = display_buffer;
  display_buffer      = drawing_buffer;
  drawing_buffer      = temp;
}

/** Zeros the drawing buffer as quickly as possible */
void clearDrawing()
{
  for (uint16_t i=0; i<DISPLAYBUFFERLEN; i++) drawing_buffer[i]=0xff;
}

void scrollDown()
{
  int r=0;
  for (;r<NROWS-1;r++)
    for (int c=0; c<NCOLS; c++) 
      setPixel(r,c,getPixel(r+1,c));
}

void scrollUp()
{
  int r;
  for (r=NROWS-1;r>0;r)
    for (int c=0; c<NCOLS; c++) 
      setPixel(r,c,getPixel(r-1,c));
}
