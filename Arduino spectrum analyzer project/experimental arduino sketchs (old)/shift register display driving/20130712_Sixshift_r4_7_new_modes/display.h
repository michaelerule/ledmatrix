#include <avr/io.h>
#include <stdint.h>   // needed for uint8_t
#include <arduino.h>

// should only run 5.8 LEDs at a time ideally
// logic is inverted so that 0 is on and 1 is off
// all drivers are on PORTB
// clock and latch are on D
// button is also on D at 7
// also RX TX are at D0 and D1
// data are sent on rising clock edges
// the column registers come first

// greyscale color bit depth 
#define BITDEPTH 3

// eventually becomes display lib include
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

// these are the data lines in terms of arduino pin numbers
#define DL0 8
#define DL1 9
#define DL2 10
#define DL3 11
#define DL4 12
#define DL5 13

// the data lines are all on portB, these are their bit addresses
#define DL0PORTB 0
#define DL1PORTB 1
#define DL2PORTB 2
#define DL3PORTB 3
#define DL4PORTB 4
#define DL5PORTB 5

const uint8_t dataLines[NDATALINES] = {DL0,DL1,DL2,DL3,DL4,DL5};
const uint8_t displayCols[3] = {8,9,10};
const uint8_t displayRows[3] = {11,12,13};

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
display_buffer_type __db1[DISPLAYBUFFERLEN];
display_buffer_type __db2[DISPLAYBUFFERLEN];
display_buffer_type *drawing_buffer = &__db1[0];
display_buffer_type *display_buffer = &__db2[0];

volatile uint16_t display_scan_index = 0;

/** Sets the shift register state to "off" as quickly as possible */
void blankDisplay() 
{
  DATALINEPORT = 0xff;
  // ( smashing everything on port D as it goes )
  for (int i=0; i<=NPINSPERREGISTER; i++) {
    PORTD = 0;
    PORTD = CLOCKMASK;
  }
  PORTD = 0; 
  PORTD = LATCHMASK;
}
/** Sets the shift register state to "on" as quickly as possible */
void lightDisplay() 
{
  DATALINEPORT = 0;
  // ( smashing everything on port D as it goes )
  for (int i=0; i<=NPINSPERREGISTER; i++) {
    PORTD = 0;
    PORTD = CLOCKMASK;
  }
  PORTD = 0; 
  PORTD = LATCHMASK;
}

/** Prepares the column portion of the display buffer
 *  shh.
 */
void initializeDisplayBuffer(display_buffer_type *buffer)
{
  // since logic levels are fixed, we have to "set" the display to off by
  // writing one to all the row / column positions
  for (int i=0; i<DISPLAYBUFFERLEN; i++) 
    buffer[i]= PORTCOLUMNMASK|PORTROWMASK;
    
  for (int f=0; f<NFRAMES; f++)
  for (int c=0; c<NCOLS; c++)
  {
    uint8_t col_register = (c>>NCOLREGISTERS)+PORTCOLBITSSTART; // register ( bit position ) of this column
    uint8_t col_bit     = c&((1<<NCOLREGISTERS)-1);  // index ( byte position in sequence ) of bytes
    buffer[f * BYTESPERFRAME + c * BYTESPERCOLUMN + (7-col_bit) ] &= ~_BV(col_register);
  }
  
}
                          
const uint8_t gamma[8] = {0,1,2,3,5,7,11,15};

/** Sets pixel (r,c) to value v in buffer 
 *  The column registers come first. 
 *  Data is sent out to the registers in 8 byte chunks
 *  Logic is inverted so that a 1 on the display is off 
 *  and a 0 is on.
 */
void setBufferPixel(uint8_t r, uint8_t c, uint8_t v, display_buffer_type *buffer) 
{
  r = 19-r;
  uint8_t row_register = (r>>NCOLREGISTERS)+PORTROWBITSSTART; // register ( bit position ) of this row
  uint8_t row_bit      = 7-(r&0b111);                         // index ( byte position in sequence ) of bytes  
  
  for (int f=0; f<NFRAMES; f++)
  { 
    if (v&1) buffer[ f*BYTESPERFRAME + c * BYTESPERCOLUMN + row_bit ] &= ~_BV(row_register);
    else     buffer[ f*BYTESPERFRAME + c * BYTESPERCOLUMN + row_bit ] |=  _BV(row_register);
    v >>= 1;
  }
}

/** Gets the pixel state at location (r,c) in buffer.
 *  e.g.
 *  uint8_t pixelState = getBufferPixel(3,7,display_buffer);
 */
uint8_t getBufferPixel(int r, int c, display_buffer_type *buffer) 
{
  r = 19-r;
  uint8_t row_register = (r>>NCOLREGISTERS)+PORTROWBITSSTART; // register ( bit position ) of this row
  uint8_t row_bit      = 7-(r&0b111);                         // index ( byte position in sequence ) of bytes  
  uint8_t v = 0;
  display_buffer_type *columnstate = &buffer[c*BYTESPERCOLUMN];
  for (int f=0; f<NFRAMES; f++)
  {
    v |= (((columnstate[row_bit]>>row_register)&1)^1)<<f;
    columnstate += BYTESPERFRAME;
  }
  return v;  
}

/** Macro to setBufferPixel for setting drawing buffer pixels */
#define setPixel(r,c,v) setBufferPixel(r,c,v,drawing_buffer)

/** Macro to getBufferPixel for getting data from display buffer */
#define getPixel(r,c) getBufferPixel(r,c,display_buffer)

/** The (current) hardware consists of 18 columns by 20 rows
 *  We will scan by column. We chopped all the registers apart
 *  so we can't simulate a decade counter anymore. That is fine. 
 *  What we have are six shift registers. We will use six bytes
 *  to store the state of these registers, and worry about 
 *  maintaining that state elsewhere. We need 18 ( number of 
 *  columns ) six-byte vectors to store a single frame. 
 *  
 */
volatile int current_frame         = 0;
volatile int current_column        = 0;
volatile int current_frame_counter = 0;
//void scanDisplay() 
ISR(TIMER0_COMPA_vect, ISR_NOBLOCK)
{
  display_buffer_type *state_vector = &display_buffer[current_frame*BYTESPERFRAME + current_column*BYTESPERCOLUMN];
  
  for (int i=0; i<BYTESPERCOLUMN; i++)
  {
    PORTB = state_vector[i];
    PORTD = 0;
    PORTD = CLOCKMASK;
  }
  PORTD = 0; 
  
  if (++current_column>=NCOLS) {
    // advance to the next column
    current_column = 0;
    // we display each "frame" for exponentially longer to create PWM
    if (++current_frame_counter>=(1<<current_frame)) {
      current_frame_counter = 0;
      // advance to the next frame
      if (++current_frame>=BITDEPTH) current_frame = 0;      
    }
  }
  PORTD = LATCHMASK; 
}

/** Initial display configuration */
void init_display()
{
  // set up shift registers for output
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);
  for (int i=0; i<NDATALINES; i++)
    pinMode(dataLines[i], OUTPUT);
  blankDisplay();
  initializeDisplayBuffer(drawing_buffer);
  initializeDisplayBuffer(display_buffer);
  
  // set up display interrupts
  //TIMER1_COMPA_vect
  
  TIMSK0 = 2;  // Timer CompA interupt 1
  TCCR0B = 2;  // speed ( 2 works, 4 is slower, this is prescaler? )
  TCCR0A = 2;  // CTC mode
  OCR0A  = 96;// period
  sei();

}


/** Flips the drawing and display buffers. 
 *  Contents of drawing buffer will be shown on screen
 *  Contents of screen will be moved to drawing buffer
 *  call clearDrawing() if you want to quickly start with a blank canvas
 *  after using this function
 */
void show() 
{
  display_buffer_type *temp = display_buffer;
  display_buffer = drawing_buffer;
  drawing_buffer = temp;
}

/** Zeros the drawing buffer as quickly as possible */
void clearDrawing()
{
  initializeDisplayBuffer(drawing_buffer);
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
