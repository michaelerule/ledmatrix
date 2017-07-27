

#ifndef debug_numerals_h
#define debug_numerals_h

#include "SpectrumDisplay.h"
#include <avr/pgmspace.h>


PROGMEM prog_uint16_t debug_numerals[16] = { 
0b111111000111111,
0b11111,
0b1011111101,
0b1111110101,
0b111110010000111,
0b1110110111,
0b111011010111111,
0b000110010111001,
0b111111010111111,
0b111110010100111,
0b111110010111111,
0b010101010111111,
0b1000111111,
0b011101000111111,
0b1010111111,
0b0010111111
};

#define getDigit(D) (pgm_read_word(&debug_numerals[(D)]))

int drawDigit(int row, int col, int digit) {
	digit %= 16;
	uint16_t digitdata = getDigit(digit);
	uint8_t  ci = 0;
	for (int c=0;c<3;c++) {
		uint8_t coldata = 0b11111&(uint8_t)(digitdata>>(5*c));
		if (coldata)
		{
			for (int r=0;r<5;r++) 
				setPixel( row+r, col+c, ((coldata>>r)&1)?WHITE:BLACK );
			ci++;
		}
	}
	return ci;
}

void drawUInt8(int row, int col, uint8_t value)
{
	if (value>=100)
		col += 1 + drawDigit(row,col,(value/100)%10);
	if (value>=10)
		col += 1 + drawDigit(row,col,(value/10)%10);
	drawDigit(row,col,(value)%10);
}

void drawUInt8Hex(int row, int col, uint8_t value)
{
	col += 1 + drawDigit(row,col,value>>4);
	drawDigit(row,col,value&0b1111);
}

void testDigits() {
	int row = 0;
	int col = 2;
	for (int d=0; d<16; d++) {
		col += 1 + drawDigit(row,col,d);
		if (col+3>=NCOLS) {
			row += 6;
			col = 2;
		}
	}
}

#endif //debug_numerals_h


