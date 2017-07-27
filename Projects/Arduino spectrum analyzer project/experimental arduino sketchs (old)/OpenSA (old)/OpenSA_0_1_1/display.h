#include <avr/io.h>
#include <stdint.h>   // needed for uint8_t
#include <arduino.h>
#include <interrupt.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#define REFRESHINTERVAL 96
#define NCOLS 18
#define NROWS 20
#define BITDEPTH 3
#define NCOLOR 6
#define WHITE (NCOLOR-1)
#define BLACK 0
#define LATCHPIN 2 //Pin connected to ST_CP of 74HC595 on PORTD
#define CLOCKPIN 3 //Pin connected to SH_CP of 74HC595
#define LATCHPINPORTD 2
#define CLOCKPINPORTD 3
#define LATCHMASK _BV(LATCHPINPORTD)
#define CLOCKMASK _BV(CLOCKPINPORTD)
#define LATCHCLOCKMASK (LATCHMASK|CLOCKMASK)
#define DATALINEPORT  PORTB
#define CLOCKLINEPORT PORTD
#define NPIX  (NROWS*NCOLS)
#define NPINSPERREGISTER 8
#define NDATALINES 6
#define PORTCOLUMNMASK 0b00000111
#define PORTROWMASK    0b00111000
#define PORTCOLBITSSTART 0
#define PORTROWBITSSTART 3
#define NCOLREGISTERS 3
#define NROWREGISTERS 3
#define display_buffer_type uint8_t
#define BYTESPERCOLUMN 8
#define NFRAMES BITDEPTH
#define BYTESPERFRAME (NCOLS*BYTESPERCOLUMN)
#define DISPLAYBUFFERLEN (BYTESPERFRAME*BITDEPTH)
#define CLOCKTICK { PORTD = 0; PORTD = CLOCKMASK; }
#define LATCHTICK { PORTD = 0; PORTD = LATCHMASK; }

const uint8_t gamma[8] = {0,1,2,3,5,7};
const uint8_t dataLines[NDATALINES] = {8,9,10,11,12,13};

display_buffer_type displayMemory[2*DISPLAYBUFFERLEN];
display_buffer_type *drawing_buffer = &displayMemory[0];
display_buffer_type *display_buffer = &displayMemory[DISPLAYBUFFERLEN];

void blankDisplay() {
  DATALINEPORT = 0xff;
  for (int i=0; i<=NPINSPERREGISTER; i++) CLOCKTICK;
  LATCHTICK;
}

void lightDisplay() {
  DATALINEPORT = 0;
  for (int i=0; i<=NPINSPERREGISTER; i++) CLOCKTICK;
  LATCHTICK;
}

void initializeDisplayBuffer(display_buffer_type *buffer) {
  for (int i=0; i<DISPLAYBUFFERLEN; i++) buffer[i]= PORTCOLUMNMASK|PORTROWMASK;    
  for (int f=0; f<NFRAMES; f++)
  for (int c=0; c<NCOLS; c++) {
    uint8_t col_register = (c>>NCOLREGISTERS)+PORTCOLBITSSTART;
    uint8_t col_bit      = c&((1<<NCOLREGISTERS)-1); 
    buffer[f * BYTESPERFRAME + c * BYTESPERCOLUMN + (7-col_bit) ] &= ~_BV(col_register);
  }
}

void setBufferPixel(uint8_t r, uint8_t c, uint8_t v, display_buffer_type *buffer) {
  r = 19-r;
  uint8_t row_register = (r>>NCOLREGISTERS)+PORTROWBITSSTART; // register ( bit position ) of this row
  uint8_t row_bit      = 7-(r&0b111);                         // index ( byte position in sequence ) of bytes  
  for (int f=0; f<NFRAMES; f++) { 
    if (v&1) buffer[ f*BYTESPERFRAME + c * BYTESPERCOLUMN + row_bit ] &= ~_BV(row_register);
    else     buffer[ f*BYTESPERFRAME + c * BYTESPERCOLUMN + row_bit ] |=  _BV(row_register);
    v >>= 1;
  }
}

uint8_t getBufferPixel(int r, int c, display_buffer_type *buffer) {
  r = 19-r;
  uint8_t row_register = (r>>NCOLREGISTERS)+PORTROWBITSSTART; // register ( bit position ) of this row
  uint8_t row_bit      = 7-(r&0b111);                         // index ( byte position in sequence ) of bytes  
  uint8_t v = 0;
  display_buffer_type *columnstate = &buffer[c*BYTESPERCOLUMN];
  for (int f=0; f<NFRAMES; f++) {
    v |= (((columnstate[row_bit]>>row_register)&1)^1)<<f;
    columnstate += BYTESPERFRAME;
  }
  return v;  
}

#define setPixel(r,c,v) setBufferPixel(r,c,v,drawing_buffer)
#define getPixel(r,c) getBufferPixel(r,c,display_buffer)

volatile int current_frame         = 0;
volatile int current_column        = 0;
volatile int current_frame_counter = 0;

ISR(TIMER0_COMPA_vect, ISR_NOBLOCK) {
  display_buffer_type *state_vector = &display_buffer[current_frame*BYTESPERFRAME + current_column*BYTESPERCOLUMN];
  for (int i=0; i<BYTESPERCOLUMN; i++) {
    PORTB = state_vector[i];
    PORTD = 0;
    PORTD = CLOCKMASK;
  }
  PORTD = 0;   
  if (++current_column>=NCOLS) {
    current_column = 0;
    if (++current_frame_counter>=(1<<current_frame)) {
      current_frame_counter = 0;
      if (++current_frame>=BITDEPTH) current_frame = 0;      
    }
  }
  PORTD = LATCHMASK; 
}

/** Initial display configuration */
void init_display() {
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);
  for (int i=0; i<NDATALINES; i++) pinMode(dataLines[i], OUTPUT);
  blankDisplay();
  initializeDisplayBuffer(drawing_buffer);
  initializeDisplayBuffer(display_buffer);
  TIMSK0 = 2;  // Timer CompA interupt 1
  TCCR0B = 2;  // speed ( 2 works, 4 is slower, this is prescaler? )
  TCCR0A = 2;  // CTC mode
  OCR0A  = REFRESHINTERVAL; // period
  sei();
}

void show() 
{
  display_buffer_type *temp = display_buffer;
  display_buffer = drawing_buffer;
  drawing_buffer = temp;
}

void clearDrawing() {
  initializeDisplayBuffer(drawing_buffer);
}

void scrollDown() {
  int r=0;
  for (;r<NROWS-1;r++)
    for (int c=0; c<NCOLS; c++) 
      setPixel(r,c,getPixel(r+1,c));
}

void scrollUp() {
  int r;
  for (r=NROWS-1;r>0;r)
    for (int c=0; c<NCOLS; c++) 
      setPixel(r,c,getPixel(r-1,c));
}

void diagonal() {
  for (int r=0;r<NROWS;r++)
  for (int c=0;c<NCOLS;c++)
  if (r==c) setPixel(r,c,WHITE);
  show();
}

void rowGreyscale() {
  for (int r=0;r<NROWS;r++)
  for (int c=0;c<NCOLS;c++)
  setPixel(r,c,r);
  show();
}

void chex() {
  for (int r=0;r<NROWS;r++)
  for (int c=0;c<NCOLS;c++)
  if ((r^c)&1)
    setPixel(r,c,WHITE);
  show();
}

void allOn() { 
   for (int c=0;c<NCOLS;c++) 
     for (int r=0;r<NROWS;r++) //if(c!=6 || r==)
       setPixel(r,c,WHITE);
  show();
}
