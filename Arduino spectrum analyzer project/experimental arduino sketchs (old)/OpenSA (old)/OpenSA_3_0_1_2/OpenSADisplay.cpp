#include <OpenSADisplay.h>
/** 
 * Open Spectrum Analizer Project
 * File created 2013-7-13, Michael Rule
 * 
 * The display library represents a single display object. Multiple 
 * display objects are not supported. Rather than implement this as a 
 * singleton, all data and methods are static. A dummy instance of 
 * OpenSADisplay can be instantiated and used to call through
 * to the methods as if it were a singleton instance. 
 * 
 */

const uint8_t OpenSADisplay::GAMMA_CORRECTED[NCOLOR] = {0,1,2,3,5,7};
const uint8_t OpenSADisplay::dataLines[NDATALINES]   = {8,9,10,11,12,13};

displayType  OpenSADisplay::displayMemory[DISPLAYBUFFERLEN*2] = {};
displayType *OpenSADisplay::draw = &OpenSADisplay::displayMemory[0];
displayType *OpenSADisplay::disp = &OpenSADisplay::displayMemory[DISPLAYBUFFERLEN];

volatile int OpenSADisplay::current_frame         = 0;
volatile int OpenSADisplay::current_column        = 0;
volatile int OpenSADisplay::current_frame_counter = 0;

#define PORTCOLUMNMASK   0b00000111
#define PORTROWMASK      0b00111000
#define PORTCOLBITSSTART 0
#define PORTROWBITSSTART 3
#define NCOLREGISTERS    3
#define NROWREGISTERS    3
void OpenSADisplay::initializeDisplayBuffer(displayType *buffer) {
  for (int i=0; i<DISPLAYBUFFERLEN; i++) 
    buffer[i]=0b111111;//PORTCOLUMNMASK|PORTROWMASK;
  for (int f=0; f<NFRAMES; f++) 
  for (int c=0; c<NCOLS  ; c++) {
    uint8_t col_register = (c>>NCOLREGISTERS)+PORTCOLBITSSTART; // register ( bit position ) of this column
    uint8_t col_bit     = c&((1<<NCOLREGISTERS)-1);  // index ( byte position in sequence ) of bytes
    buffer[f * BYTESPERFRAME + c * BYTESPERCOLUMN + (7-col_bit) ] &= ~_BV(col_register);
  }
}
                          
void OpenSADisplay::setBufferPixel(uint8_t r, uint8_t c, uint8_t v, displayType *buffer) {
  r = 19-r;
  uint8_t row_register = (r>>NCOLREGISTERS)+PORTROWBITSSTART; // register ( bit position ) of this row
  uint8_t row_bit      = 7-(r&0b111);                         // index ( byte position in sequence ) of bytes  
  for (int f=0; f<NFRAMES; f++) { 
    if (v&1) buffer[ f*BYTESPERFRAME + c * BYTESPERCOLUMN + row_bit ] &= ~_BV(row_register);
    else     buffer[ f*BYTESPERFRAME + c * BYTESPERCOLUMN + row_bit ] |=  _BV(row_register);
    v >>= 1;
  }
}

void OpenSADisplay::setBufferPixelColor(uint8_t r, uint8_t c, color v, displayType *buffer)  {
  v = v>=NCOLOR?NCOLOR-1:v<0?0:v;
  v = GAMMA_CORRECTED[v];
  setBufferPixel(r,c,v,buffer);
}

uint8_t OpenSADisplay::getBufferPixel(uint8_t r, uint8_t c, displayType *buffer)  {
  r = 19-r;
  uint8_t row_register = (r>>NCOLREGISTERS)+PORTROWBITSSTART; // register ( bit position ) of this row
  uint8_t row_bit      = 7-(r&0b111);                         // index ( byte position in sequence ) of bytes  
  uint8_t v = 0;
  displayType *columnstate = &buffer[c*BYTESPERCOLUMN];
  for (int f=0; f<NFRAMES; f++) {
    v |= (((columnstate[row_bit]>>row_register)&1)^1)<<f;
    columnstate += BYTESPERFRAME;
  }
  return v;  
}

/**
 *  Display scanning is tricky.
 *  There are six shift registers that muts be written in parallel off
 *  PORTB. The first three (PB0..2) are for column data vectors (20)
 *  The last three (PB3..5) are for the column index drivers. In theory
 *  This is all managed separately for us and we just need to send the
 *  bytes over.
 */
void OpenSADisplay::scan() {
  displayType *state_vector = &draw[current_frame*BYTESPERFRAME + current_column*BYTESPERCOLUMN];
  for (int i=0; i<BYTESPERCOLUMN; i++) {
    PORTB = state_vector[i];
    CLOCKTICK;
  }
  LATCHTICK;
  if (++current_column>=NCOLS) {
    current_column = 0;
    if (++current_frame_counter>=(1<<current_frame)) {
      current_frame_counter = 0;
      if (++current_frame>=BITDEPTH) current_frame = 0;      
    }
  }
}


ISR(TIMER0_COMPA_vect, ISR_NOBLOCK)
{
	OpenSADisplay::scan();
}

void OpenSADisplay::init() {
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);
  for (int i=0; i<NDATALINES; i++)
    pinMode(dataLines[i], OUTPUT);
  initializeDisplayBuffer(draw);
  initializeDisplayBuffer(disp);
  blank();
  /*
  TIMSK0 = 2;  // Timer CompA interupt 1
  TCCR0B = REFRESH_PRESCALER;  // speed ( 2 works, 4 is slower, this is prescaler? )
  TCCR0A = 2;  // CTC mode
  OCR0A  = REFRESH_INTERVAL;// period
  sei();
  */
}

void OpenSADisplay::down() {
  int r=0;
  for (;r<NROWS-1;r++)
    for (int c=0; c<NCOLS; c++) 
      setPixel(r,c,getPixel(r+1,c));
}

void OpenSADisplay::up() {
  int r;
  for (r=NROWS-1;r>0;r)
    for (int c=0; c<NCOLS; c++) 
      setPixel(r,c,getPixel(r-1,c));
}

void  OpenSADisplay::show()  { displayType *temp = disp; disp = draw; draw = temp; }
void  OpenSADisplay::blank() { DATAPORT=~0; for (int i=0; i<=8; i++) CLOCKTICK; LATCHTICK; }
void  OpenSADisplay::light() { DATAPORT= 0; for (int i=0; i<=8; i++) CLOCKTICK; LATCHTICK; }
void  OpenSADisplay::setPixel(uint8_t r,uint8_t c,uint8_t v) { setBufferPixel(r,c,v,draw); }
void  OpenSADisplay::setColor(uint8_t r,uint8_t c,uint8_t v) { setBufferPixelColor(r,c,v,draw); }
color OpenSADisplay::getPixel(uint8_t r,uint8_t c)           { getBufferPixel(r,c,disp); }
void  OpenSADisplay::clearDrawing()  { initializeDisplayBuffer(draw); }
	
	
	
