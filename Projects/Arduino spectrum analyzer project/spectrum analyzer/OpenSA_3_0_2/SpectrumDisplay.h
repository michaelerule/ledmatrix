/**
 * Display driver for Open Spectrum Analyzer (please see SpectrumGraphics.h).
 * @file   SpectrumDisplay.h
 * @date   August, 2013
 * 
 * @copyright Michael Rule (mrule7404@gmail.com) 2013
 * @copyright Joshua  Holt (jholt7532@gmail.com) 2013
 * 
 * This file is part of Open Spectrum Analyzer (OpenSA).
 * 
 * OpenSA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * OpenSA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with OpenSA.  If not, see <http://www.gnu.org/licenses/>. 
 */

#ifndef spectrum_display_h
#define spectrum_display_h

#include <stdint.h>
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define NCOLS            24         //!< Number of columns in the display
#define NROWS            20         //!< Number of rows in the display
#define REFRESHINTERVAL  200        //!< For CTC interrupt at OCR0A. Driver triggers every REFRESHINTERVAL tics.
#define LATCHPIN         2          //!< Pin connected to ST_CP of 74HC595 ( in terms of Arduino mapping )
#define CLOCKPIN         3          //!< Pin connected to SH_CP of 74HC595 ( in terms of Arduino mapping )
#define DATALINEPORT     PORTB      //!< Port used for the datalines. This port gets smashed, don't put anything else on it.
#define CLOCKLINEPORT    PORTD      //!< Port used for the clock lines. 
#define LATCHMASK        _BV(2)     //!< Bit mask for the latch clock line
#define CLOCKMASK        _BV(3)     //!< Bit mask for the data  clock line
#define NPINSPERREGISTER 8          //!< number of parallel outputs per shift register
#define NDATALINES       6          //!< number of data lines ( shift registers )
#define PORTCOLBITSSTART 0          //!< starting index of registers used for column data
#define PORTROWBITSSTART 3          //!< starting index of registers used for column scanning
#define NCOLREGISTERS    3          //!< number of registers used for column data
#define BITDEPTH         2          //!< greyscale bit depth of internal display memory
#define NFRAMES          BITDEPTH   //!< an alias for bit depth, related to how PWM is achieved
#define PORTCOLUMNMASK   0b00000111 //!< a mask for the data lines of registers related to column data
#define PORTROWMASK      0b00111000 //!< a mask for the data lines of registers related to column scanning
#define display_buffer_type uint8_t //!< data type used to store display data
#define BYTESPERCOLUMN   4          //!< number of display_buffer_type needed to store column data

#define BYTESPERFRAME (NCOLS*BYTESPERCOLUMN)        //!< number of display_buffer_type needed to store one frame
#define DISPLAYBUFFERLEN (BYTESPERFRAME*BITDEPTH)   //!< number of display_buffer_type needed for display memory
#define CLOCKTICK { PORTD = 0; PORTD = CLOCKMASK; } //!< shortcut for triggering data clock 
#define LATCHTICK { PORTD = 0; PORTD = LATCHMASK; } //!< shortcut for triggering data latch

/* display memory

total bytes needed is 2*NCOLS*BYTESPERCOLUMN*BITDEPTH

2*24*4*3
*/

const uint8_t dataLines[NDATALINES] = {8,9,10,11,12,13}; //!< internal display handling only.

display_buffer_type displayMemory[2*DISPLAYBUFFERLEN]; //!< internal display handling only. display memory allocation

display_buffer_type *drawing_buffer = &displayMemory[0]; //!< internal display handling only. drawing buffer
display_buffer_type *display_buffer = &displayMemory[DISPLAYBUFFERLEN]; //!< internal display handling only. displayed buffer

volatile uint8_t current_frame         = 0; //!< internal display handling only. tracks the current frame for PWM
volatile uint8_t current_column        = 0; //!< internal display handling only. tracks current column
volatile uint8_t current_frame_counter = 0; //!< internal display handling only. tracks overscan of current frame for PWM

// Functions can be registered with the display module and will be polled
// in synch with the display. This allows minor processes like button 
// polling to be handled occasionally by the display module without 
// creating additional interrupts for them. 
void (*display_callback)() = 0; //!< register a callback function that can be polled in time with the display

/**
 * Display timer interrupt handler
 *
 * Six shift registers have their data lines hooked in parallel up to PORTB
 * PORTB 6 and 7 are used for the XTAL and are not active, only the lower
 * 6 lines are relevant. 
 * 
 * The lower three are hooked up to the column drivers
 * The upper three are hooked up to the rows to send column data
 * 
 * If it seems like columns 16 and 17 aren't lighting check to see 
 * whether you are piping the data into register 2 "backwards" -- they are
 * hooked up to the first two pins, which means they get the lase two 
 * bytes since data is sent first in last out.
 * 
 * Logic high turns pixles off, FYI
 */

  
ISR(TIMER0_COMPA_vect, ISR_NOBLOCK) {
  display_buffer_type *state_vector = &display_buffer[current_frame*BYTESPERFRAME + current_column*BYTESPERCOLUMN];
  
  uint8_t column_scan_index = ~current_column & 7;
  uint8_t column_i          = 0 ;
  uint8_t column_mask       = 1<<(current_column>>3);
  for (int i=0; i<4; i++) 
  {
	uint8_t data = state_vector[i];
	
    PORTB = (data | 7) ^ (column_i == column_scan_index? column_mask : 0);
    PORTD = 0;
    PORTD = CLOCKMASK;
	column_i ++;
	
	data <<= 3;
    PORTB = (data | 7) ^ (column_i == column_scan_index? column_mask : 0);
    PORTD = 0;
    PORTD = CLOCKMASK;
	column_i ++;
  }
  
  if (++current_column>=NCOLS) {
    current_column = 0;
    if (++current_frame_counter>=(1<<current_frame)) {
      current_frame_counter = 0;
      if (++current_frame>=BITDEPTH) {
      	current_frame = 0;      
      	if (display_callback) display_callback();
  	  }
    }
  }
  PORTD = LATCHMASK; 
}

/**
 * Wipes the shift registers for a blank display
 */
void blankDisplay() {
  DATALINEPORT = 0xff;
  for (int i=0; i<=NPINSPERREGISTER; i++) CLOCKTICK;
  LATCHTICK;
}

/** 
 * Configures display memory 
 */
void initializeDisplayBuffer(display_buffer_type *buffer) 
{
  for (int i=0; i<DISPLAYBUFFERLEN; i++) buffer[i]=0xff;
}

/**
 * Sets pixel data for display or drawing buffer
 */
void setBufferPixel(uint8_t r, uint8_t c, uint8_t v, display_buffer_type *buffer) {
  //r = NROWS+1-r;
  uint8_t row_register = (r>>NCOLREGISTERS); // register ( bit position ) of this row
  uint8_t row_bit      = 7-(r&0b111);        // index ( byte position in sequence ) of bytes  
  row_register += 3-(row_bit&1)*3;
  row_bit >>= 1;
  for (int f=0; f<NFRAMES; f++) {
    if (v&1) buffer[ f*BYTESPERFRAME + c*BYTESPERCOLUMN + row_bit ] &= ~_BV(row_register);
    else     buffer[ f*BYTESPERFRAME + c*BYTESPERCOLUMN + row_bit ] |=  _BV(row_register);
    v >>= 1;
  }
}

/**
 * Retrieves pixel data from display or drawing buffer
 */
uint8_t getBufferPixel(uint8_t r, uint8_t c, display_buffer_type *buffer) {
  //r = NROWS+1-r;
  uint8_t row_register = (r>>NCOLREGISTERS); // register ( bit position ) of this row
  uint8_t row_bit      = 7-(r&0b111);        // index ( byte position in sequence ) of bytes  
  row_register += 3-(row_bit&1)*3;
  row_bit >>= 1;
  uint8_t v = 0;
  display_buffer_type *columnstate = &buffer[c*BYTESPERCOLUMN];
  for (int f=0; f<NFRAMES; f++) {
    v |= (((columnstate[row_bit]>>row_register)&1)^1)<<f;
    columnstate += BYTESPERFRAME;
  }
  return v;  
}

/**
 * Initial display configuration.
 */
void init_display() {
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);
  for (int i=0; i<NDATALINES; i++) pinMode(dataLines[i], OUTPUT);
  blankDisplay();
  initializeDisplayBuffer(drawing_buffer);
  initializeDisplayBuffer(display_buffer);
  TIMSK0 = 2;  // Timer CompA interupt 1
  TCCR0B = 2;  // speed prescaler ( 2 works, 4 is slower )
  TCCR0A = 2;  // CTC mode
  OCR0A  = REFRESHINTERVAL; // period
  sei();
}

#endif
