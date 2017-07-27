/**
 * Font rendering for OpenSA.
 * @file   SpectrumFont.h
 * @date   August, 2013
 * 
 * A limited number of system fonts have been rasterized and converted to
 * bit-packed representations in the fonts directory. This library supports
 * rendering one of those fonts.
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


#ifndef spectrum_text_h
#define spectrum_text_h

#include "./font/liberation_sans_plain_20.h"

/**
 *
 */
void drawColum(int16_t x, int16_t y, uint16_t start) {
	int8_t k=0;
	for (int8_t b=0; b<font_nbytes; b++) 
	{
		uint8_t colData = pgm_read_byte(&font_data[b+start]);
		for (uint8_t j=0; j<8; j++) 
		{
			setColor( font_height-1-k+y, x, colData>>(7-j)&1? WHITE:BLACK );
			if (++k>=font_height) 
				break;
		}
	}
}

/**
 *
 */
uint8_t drawChar(int16_t x, int16_t y, char ch) {
	if (ch==' ') return 5;
	ch -= 33;
	uint8_t  charWidth = pgm_read_byte(&font_glyphWidth[ch]);
	uint16_t charStart = pgm_read_word(&font_glyphStart[ch]);
	for (uint8_t i=0; i<charWidth; i++) 
		drawColum( x+i, y, font_nbytes*(i+charStart) );
	return charWidth;
}

/**
 *
 */
void drawString(int16_t x, int16_t y, char *s)
{
	for (int16_t i=0; s[i]; i++)
		x += drawChar(x,y,s[i])+2;
}

#endif


