
#include "alphadata.h"

void drawInt(int8_t dr,int8_t dc,uint8_t digit)
{
	int8_t r,c;
	for (r=0;r<5;r++) for (c=0;c<4;c++) set(buff,r+dr,c+dc,get(disp,r+dr,c+dc));
	for (c=0;c<3;c++) {
		uint8_t columndata = pgm_read_byte_near( &alphabet[3*digit+c]);
		for (r=0;r<5;r++) set(disp,r+dr,c+dc,(columndata>>r)&1);
	}
	for (r=0;r<5;r++)  set(disp,r+dr,c+dc,0);
}

void clearInt(int8_t dr,int8_t dc)
{
	int8_t r,c;
	for (r=0;r<5;r++) for (c=0;c<4;c++) set(disp,r+dr,c+dc,get(buff,r+dr,c+dc));
}

void drawDecimal( uint8_t number )
{
	uint8_t tens = (number/10)%10;
	uint8_t ones = number%10;
	drawInt(1,0,tens);
	drawInt(1,4,ones);
}

void clearDecimal( uint8_t number )
{
	clearInt(1,0);
	clearInt(1,4);
}
