/** 
 * Open Spectrum Analizer Project
 * File created 2013-7-13, Michael Rule
 * 
 * 
 */

#ifndef OpenSADisplay_h
#define OpenSADisplay_h

#define REFRESH_PRESCALER 2  // divides the system clock to determine the display scanning clock ( 2 works, 4 is slower )
#define REFRESH_INTERVAL  37 // determines how many prescaled clock ticks to wait before scanning the display
#define BITDEPTH          3  // number of color bits in the internal display representation
#define NCOLOR            10 // number colors, mapped to bit representation by the GAMMA_CORRECTED variable
const uint8_t GAMMA_CORRECTED[NCOLOR] = {0,1,2,3,5,7,9,15,20,31};

#define LATCHPIN  2     //Pin connected to data latch clock of 74HC595 on PORTD ( rise triggered )
#define CLOCKPIN  3     //Pin connected to data line  clock of 74HC595 on PORTD ( rise triggered )
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


const uint8_t dataLines[NDATALINES] = {DL0,DL1,DL2,DL3,DL4,DL5};
const uint8_t displayCols[3] = {8,9,10};
const uint8_t displayRows[3] = {11,12,13};
display_buffer_type __db1[DISPLAYBUFFERLEN];
display_buffer_type __db2[DISPLAYBUFFERLEN];
display_buffer_type *drawing_buffer = &__db1[0];
display_buffer_type *display_buffer = &__db2[0];
volatile uint16_t display_scan_index = 0;
void blankDisplay() 
void lightDisplay() 
void initializeDisplayBuffer(display_buffer_type *buffer)
const uint8_t gamma[8] = {0,1,2,3,5,7,11,15};
void setBufferPixel(uint8_t r, uint8_t c, uint8_t v, display_buffer_type *buffer) 
uint8_t getBufferPixel(int r, int c, display_buffer_type *buffer) 
void setPixel(r,c,v)
uint8_t getPixel(r,c)
volatile int current_frame         = 0;
volatile int current_column        = 0;
volatile int current_frame_counter = 0;
void scan()
void init_display()
void show() 
void clearDrawing()
void scrollDown()
void scrollUp()

#endif //OpenSADisplay_h

