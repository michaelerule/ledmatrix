#ifndef braille_patterns_h
#define braille_patterns_h

#include "SpectrumDisplay.h"
#include <avr/pgmspace.h>

PROGMEM prog_uint8_t braille_alphabet[26] = { 
000001
000011
001001
011001
010001

001011
011011
010011
001010
011010

000101
000111
001101
011101
010101

001111
011111
010111
001110
011110

011101
100111
111010
101101
111101

110101

#endif //braille_patterns_h

