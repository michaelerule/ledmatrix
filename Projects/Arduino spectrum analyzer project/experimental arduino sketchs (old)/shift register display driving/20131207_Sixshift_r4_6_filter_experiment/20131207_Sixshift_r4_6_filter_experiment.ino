#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>   // needed for uint8_t

#define NCOLS 18
#define NROWS 20
#include "display.h"
#include "displayTests.h"
#include "audio.h"

void setup() {
  init_display();
  chex();
  init_audio();
}

#define ALPHA 0
#define LIRP(ALPHA,A,B) ((A)*(ALPHA)+(256-(ALPHA))*(B)>>8)

void loop()
{
  uint8_t r,c;
  for (c=0;c<18;c++)
  {
    uint8_t level = filteroutput[c];//levelsR[c] = LIRP(ALPHA,levelsR[c],filteroutput[c]);
    level /= 13;
    for (r=0;r<level;r++)
      setPixel(r,c,0xff);
    for (   ;r<NROWS;r++)
      setPixel(r,c,0);
  }
  show();
}


