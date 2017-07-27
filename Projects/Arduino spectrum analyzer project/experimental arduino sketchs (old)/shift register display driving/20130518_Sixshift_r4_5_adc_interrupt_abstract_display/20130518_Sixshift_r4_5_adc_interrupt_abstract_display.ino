#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>   // needed for uint8_t

#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point fht
#include <FHT.h>  // include the FHT library

#define NCOLS 18
#define NROWS 20
#include "display.h"

#include "audio.h"

#define sensePot  A2
#define NMODES    4
#define buttonPin 12
#include "controls.h"

void setup() {
  init_display();
  init_controls();
  init_audio();
}

void loop()
{
    if (check_audio())
    {
      setLevels(current_channel?&levelsR[0]:&levelsL[0]); // set the screen state
      drawLevels();
    }
    
    uint8_t c;
    for (c=0;c<NCOLS;c++)
    scanDisplay();
    blankDisplay();
    
    check_buttons(); 
}

const uint8_t fft_monitor[] = {3,7,11,15,19,23,27,31,35,39,43,47,51,55,59,63};

void setLevels(int *levels)
{
  long averageL=0;
  long averageR=0;
  for(int x=2; x <   9; x++) averageL += levels[x];
  for(int y=9; y <= 18; y++) averageR += levels[y];
  levels[17] = averageL/8;
  levels[16] = averageR/8;
  uint8_t i,j;
  for (i=0;i<16;i++)
  {
    uint8_t freq = fft_monitor[15-i+1];
    uint16_t average = 0;
    for (j=0;j<AVERAGESAMPLES;j++)
      average += fht_log_out[(15-i)*AVERAGESAMPLES+j];
    average *= log(freq)/4;
    average /= AVERAGESAMPLES;
    levels[i] = (levels[i]*200+56*average)>>8;
  }
}

void drawLevels()
{
  uint8_t c,r;
  clearDrawing();
  for (c=0; c<NCOLS; c++) {
    uint8_t level = (((int16_t)levelsR[c])*volume)>>8;
    for (r=0;r<level;r++)
      setPixel(r,c,1);
    for(;r<NROWS;r++)
      setPixel(r,c,0);
  } 
  show();
}



