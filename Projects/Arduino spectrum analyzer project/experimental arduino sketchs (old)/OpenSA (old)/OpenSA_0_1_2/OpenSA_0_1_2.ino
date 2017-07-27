#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>   // needed for uint8_t
#include <avr/interrupt.h>

#include <spectrum_FHT.h>  // include the FHT library

#include "display.h"
#include "audio.h"

#define sensePot  A2
#define buttonPin 7
#include "controls.h"

void setup() {
  init_display();
  chex();
  init_controls();
  init_audio();
}

void loop()
{
    if (check_audio())
    {
      setLevels(current_channel); // set the screen state
      drawLevels();
    }
    check_buttons(); 
}

const uint8_t fft_monitor[] = {3,7,11,15,19,23,27,31,35,39,43,47,51,55,59,63};

#define ALPHA 100
#define MAXALPHA 240
#define MINALPHA 200
#define LOWALPHA 240
#define LIRP(ALPHA,A,B) ((A)*(ALPHA)+(256-(ALPHA))*(B)>>8)

void setLevels(int channel)
{
  int *levels  = channel?&levelsR[0]:&levelsL[0];
  int *maximum = channel?&maxR[0]:&maxL[0];
  int *minimum = channel?&minR[0]:&minL[0];
  int *low     = channel?&lowR[0]:&lowL[0];
  long averageL=0;
  long averageR=0;
  for(int x=2; x <   9; x++) averageL += levels[x];
  for(int y=9; y <= 18; y++) averageR += levels[y];
  levels[0] = averageL/8;
  maximum[0] = levels[0]>maximum[1] ? levels[0] : LIRP(MAXALPHA,maximum[0],levels[0]);
  levels[1] = averageR/8;
  maximum[1] = levels[1]>maximum[1] ? levels[1] : LIRP(MAXALPHA,maximum[1],levels[1]);  
  uint8_t i,j;
  for (i=2;i<18;i++)
  {
    uint8_t freq = fft_monitor[15-(i-2)+1];
    uint16_t average = 0;
    for (j=0;j<AVERAGESAMPLES;j++)
    {
      uint16_t index = (15-(i-2))*AVERAGESAMPLES+j;
      average += fht_log_out[index];
    }
    average *= log(freq)/3;
    average += AVERAGESAMPLES/2;
    average /= AVERAGESAMPLES;
    low[i] = LIRP(LOWALPHA,low[i],average) ;
    int corrected = average - low[i];
    if (corrected<0) corrected = 0;
    average = corrected;
    levels[i] = LIRP(ALPHA,levels[i],average);
    average = levels[i];
    minimum[i] = average<minimum[i] ? average : LIRP(MINALPHA,minimum[i],average) ;
    maximum[i] = average>maximum[i] ? average : LIRP(MAXALPHA,maximum[i],average) ;
  }
}


