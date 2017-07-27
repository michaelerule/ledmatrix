
#include <avr/pgmspace.h>

#include "alphadata.h"

#define CHLOAD(chpos) (pgm_read_byte_near(&alphabet[chpos]))
#define CHWIDTH(x) ((x)>>5)
#define CHDATA(x) ((x)&0x1f)

uint32_t loadChar(uint8_t ch)
{
	uint32_t chardata;
	uint8_t cdat,chw;
	uint8_t chpos = 0;
	while (ch) {
		cdat  = CHLOAD(chpos);
		chpos += CHWIDTH(cdat);
		ch--;
	}
	cdat = CHLOAD(chpos);
	chw  = CHWIDTH(cdat);
	chardata = CHDATA(cdat);
	for (ch=1; ch<chw; ch++)
	{
		chardata <<= 5;
		chardata += CHDATA(CHLOAD(++chpos));
	}
	chardata |= ((uint32_t)chw)<<(32-3);
	return chardata;
}


