#ifndef spectrum_text_h
#define spectrum_text_h

#include "char.h"

uint8_t drawChar(int8_t x, int8_t y, char ch)
{
	uint8_t charIndex = pgm_read_byte(&ASCIIMap[ch]);
	if (charIndex==0xff) 
		return 0;

	uint8_t charWidth = pgm_read_byte(&charWidths[charIndex]);
	uint8_t charStart = pgm_read_byte(&charOffset[charIndex]);
	for (uint8_t i=0; i<charWidth; i++)
	{
		uint8_t colData = pgm_read_byte(&charData[i+charStart]);
		for (uint8_t j=0; j<8; j++)
			setColor( y+j, x, colData>>(7-j)&1? WHITE:BLACK );
		x++;
	}

	return charWidth;
}

void drawString(int8_t x, int8_t y, char *s)
{
	for (uint8_t i=0; s[i]; i++)
		x += drawChar(x,y,s[i]-32)+1;
}

#endif
