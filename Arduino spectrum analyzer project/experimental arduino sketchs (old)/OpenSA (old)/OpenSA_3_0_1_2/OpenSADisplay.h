#ifndef OpenSADisplay_h
#define OpenSADisplay_h
/** 
 * Open Spectrum Analizer Project
 * File created 2013-7-13, Michael Rule
 * 
 * 
 */
#include <avr/io.h>
#include <stdint.h> 
#include <interrupt.h>
#include <Arduino.h>

#define NROWS 20
#define NCOLS 18

#define REFRESH_PRESCALER 2 // The prescaler divides the system clock to determine the display scanning clock ( 2 works, 4 is slower )
#define REFRESH_INTERVAL  37 // The interval determines how many prescaled clock ticks to wait before scanning the display

#define BITDEPTH 3 // BITDEPTH sets the number of color bits to use in the internal display representation
#define NCOLOR   6 // NCOLOR sets the number of actual colors to use -- mapped to bit representation by the GAMMA_CORRECTED variable

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

#define display_dtype uint8_t
#define BYTESPERCOLUMN    3
#define NFRAMES           BITDEPTH
#define BYTESPERFRAME    (NCOLS*BYTESPERCOLUMN)
#define DISPLAYBUFFERLEN (BYTESPERFRAME*NFRAMES)

#define NOP __asm__("nop\n\t");
#define delayop(ops) {for (int __delay__ops=ops; __delay__ops!=0; __delay__ops--) NOP;}

typedef uint8_t displayType;
typedef uint8_t color;

class OpenSADisplay
{
  public:
	static void  init();
	static void  scan();
	static void  initializeDisplayBuffer(displayType *buffer);
	static void  setBufferPixel(uint8_t r, uint8_t c, color v, displayType *buffer);
	static color getBufferPixel(uint8_t r, uint8_t c, displayType *buffer);
	static void  setBufferPixelColor(uint8_t r, uint8_t c, color v, displayType *buffer);
	static void  down();
	static void  up();
	static void  show();//  { displayType *temp = disp; disp = draw; draw = temp; }
	static void  blank();// { DATAPORT=~0; for (int i=0; i<=8; i++) CLOCKTICK; LATCHTICK; }
	static void  light();// { DATAPORT= 0; for (int i=0; i<=8; i++) CLOCKTICK; LATCHTICK; }
	static void  setPixel(uint8_t r,uint8_t c,uint8_t v);// { setBufferPixel(r,c,v,draw); }
	static void  setColor(uint8_t r,uint8_t c,uint8_t v);// { setBufferPixelColor(r,c,v,draw); }
	static color getPixel(uint8_t r,uint8_t c);//           { getBufferPixel(r,c,disp); }
	static void  clearDrawing();//  { initializeDisplayBuffer(draw); }
  private:
	static displayType displayMemory[DISPLAYBUFFERLEN*2];
	static displayType *draw;
	static displayType *disp;
	static volatile int current_frame;
	static volatile int current_column;
	static volatile int current_frame_counter;
 	static const uint8_t GAMMA_CORRECTED[NCOLOR] ;
 	static const uint8_t dataLines[NDATALINES] ;
};

#endif //OpenSADisplay_h

