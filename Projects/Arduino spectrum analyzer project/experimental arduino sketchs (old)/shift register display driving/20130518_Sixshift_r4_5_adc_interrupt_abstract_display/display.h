#include <avr/io.h>
#include <stdint.h>   // needed for uint8_t
#include <arduino.h>

// eventually becomes display lib include
#define LATCHPIN 3 //Pin connected to ST_CP of 74HC595 on PORTD
#define CLOCKPIN 2 //Pin connected to SH_CP of 74HC595
#define DATAPIN 4  //Pin connected to DS    of 74HC595
#define reset  5
#define enable 6
#define NPIX  (NROWS*NCOLS)
#define NLVL  2

#define display_buffer_type uint32_t

display_buffer_type __db1[NCOLS];
display_buffer_type __db2[NCOLS];
display_buffer_type *drawing_buffer = &__db1[0];
display_buffer_type *display_buffer = &__db2[0];

volatile uint16_t display_scan_index = 0;

/** Initial display configuration */
void init_display()
{
  // set up shift registers for output
  pinMode(DATAPIN, OUTPUT);
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);

  // set up.. other stuff for output
  pinMode(enable, OUTPUT);
  pinMode(reset, OUTPUT);
  digitalWrite(enable, LOW);
  digitalWrite(reset, HIGH);
}

/** Does nothing */
void displayEnable(int b)
{
  digitalWrite(enable,!b);
}

/** Scans one row of the display. 
 *  Call NROWS times to scan whole display
 */
void scanDisplay() 
{
  // one column at a time is displayed
  // each column has 20 pixels, but three shift registers are used so it is 24 pins.
  uint32_t col = (1UL<<display_scan_index)<<(24-NCOLS);
  // we need to grab the row data
  uint32_t row = display_buffer[display_scan_index];
  // oh yeah then this happened ( logic level inversion )
  row = ~row;
  col = ~col;
  // send the pin state out to the shift registers
  // the data line is set at the same time that the clock line is cleared to zero
  // then a rising clock edge is created
  for (uint8_t i=0; i<24;i++) {
    PORTD  = 0b10000&(row>>(i-4));
    PORTD |= 0b100;
  }
  for (uint8_t i=0; i<24;i++) {
    PORTD  = 0b10000&(col>>(i-4));
    PORTD |= 0b100;
  }
  //create a rising edge on the latch pin to show this column
  PORTD = 0b1000;
  if (++display_scan_index>=NCOLS) display_scan_index=0;
}

/** Sets pixel (r,c) to value v in buffer */
void setBufferPixel(uint8_t r, uint8_t c, uint8_t v, display_buffer_type *buffer) {
  // right now the display pixels are row-packed so 
  r+=4;
  uint32_t coldata = buffer[c];
  if (v) coldata |=  (1UL<<r);
  else   coldata &=~ (1UL<<r);
  buffer[c] = coldata;
}

/** Macro to setBufferPixel for setting drawing buffer pixels */
#define setPixel(r,c,v) setBufferPixel(r,c,v,drawing_buffer)


/** Gets the pixel state at location (r,c) in buffer.
 *  e.g.
 *  uint8_t pixelState = getBufferPixel(3,7,display_buffer);
 */
uint8_t getBufferPixel(int r, int c, display_buffer_type *buffer) 
{
  return 0;
}


/** Macro to getBufferPixel for getting data from display buffer */
#define getPixel(r,c,v) getBufferPixel(r,c,v,display_buffer)


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


/** Sets the shift register state to "off" as quickly as possible */
void blankDisplay() 
{
  for (int i=0;i<24*2;i++) {
    PORTD = 0b10000;
    PORTD = 0b10100;
  }
  //create a rising edge on the latch pin to show this column
  PORTD &=~ _BV(LATCHPIN);
  PORTD |=  _BV(LATCHPIN);  
}


/** Zeros the drawing buffer as quickly as possible */
void clearDrawing()
{
  uint8_t c;
  for (c=0;c<NCOLS;c++)
    drawing_buffer[c] = 0UL;  
}


