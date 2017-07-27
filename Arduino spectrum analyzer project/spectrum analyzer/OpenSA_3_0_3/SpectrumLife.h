/**
 * Game of life mode for OpenSA
 * @file   SpectrumLife.h
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
#ifndef spectrum_life_h
#define spectrum_life_h

#include "SpectrumDisplay.h"

#define buff drawing_buffer
#define disp display_buffer

inline void set(display_buffer_type *buffer,uint8_t r,uint8_t c,uint8_t value)
{
	//setBufferPixel( r, c, value?WHITE:getBufferPixel( r, c, buffer)>>1, buffer);
	setBufferPixel( r, c, value?WHITE:0, buffer);
}

#define get(buffer,r,c) (getBufferPixel((r),(c),(buffer))?1:0)

// The game of life wraps around at the edges. To abstract away some of the
// difficulty, these previous and next functions will return the previous
// or next row or column, automatically handlign wrapping around the edges.
// For example, the next column after column 15 is column 0. The previous
// row to row 0 is row 15.

#define prevRow(x) (x? (x<NROWS? x-1:0) : NROWS-1)
#define nextRow(x) (x <(NROWS-1) ? x+1:0)
#define prevCol(x) (x? (x<NCOLS? x-1:0) : NCOLS-1)
#define nextCol(x) (x <(NCOLS-1) ? x+1:0)

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
#define columnCount(r,c) (get(disp,prevRow(r),c) + get(disp,r,c) + get(disp,nextRow(r),c))

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

// the old method of entropy harvesting from the temperature sensor
// doesn't work when we have the ADC set up for fast audio sampling
// rather, it works, but requires more messing about with config than 
// I'm willing to do at the moment.
uint16_t rngstate = 0b100110101;
uint8_t entropy() { return ((uint8_t)((rngstate ^= rngstate>>5^rngstate<<1)&0xff)); }

void init_life() {	
	uint8_t r,c;
	//GAME OF LIFE
	for (r=0;r<NROWS;r++) for (c=0;c<NCOLS;c++) set(disp,r,c,entropy()&2);
}

void life() {	
	uint8_t i,j,r,c;
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
    uint8_t changed = 0;
    uint8_t k = 0;
    for (r=0; r<NROWS; r++)
    {
        uint8_t previous = columnCount(r,NCOLS-1);
        uint8_t current  = columnCount(r,0);
        uint8_t neighbor = previous+current;
        for (c=0; c<NCOLS; c++)
        {
            uint8_t cell = get(disp,r,c);

            uint8_t upcoming = columnCount(r,nextCol(c));
            neighbor += upcoming;
            uint8_t newcell = cell? (neighbor+1>>1)==2:neighbor==3;

            neighbor-= previous;
            previous = current ;
            current  = upcoming;
            changed |= newcell!=cell && newcell!=get(buff,r,c);
            k += newcell;
            set(buff,r,c,newcell);
        }
    }
    
    uint8_t l=0;
    if (!(k&&entropy())) l=glider;
    if (entropy()<5) l=glider;
    if (!changed && shownFor++>TIMEOUT) l=glider;
    if (l) {
        uint8_t r = entropy()%NROWS;
        uint8_t q = entropy()%NCOLS;
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
                c = a?nextCol(c):prevCol(c);
            }
            r = b?nextRow(r):prevRow(r);
        }    
        shownFor = 0;
    }
    
    // flip the pixel buffers
    show();
} 



#endif
