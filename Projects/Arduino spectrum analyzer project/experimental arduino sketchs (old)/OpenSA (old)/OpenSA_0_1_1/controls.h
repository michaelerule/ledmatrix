#include <arduino.h>

#define NMODES    5
int buttonDebounce = 0;
int visualizeMode = 0;

void init_controls(){
  // button is.. input
  pinMode( buttonPin, INPUT );  
  pinMode( buttonPin, INPUT_PULLUP );
  digitalWrite(buttonPin, HIGH);
}

void check_buttons(){
    // read button with debouncing and increment visualizer more if it is triggered
    if (digitalRead(buttonPin)==LOW)
      if (!buttonDebounce)  {
        buttonDebounce = 50;
        visualizeMode = (visualizeMode+1)%NMODES;
      }
    if (buttonDebounce) buttonDebounce--; 
}

void drawLevels() {
  uint8_t c,r;
  clearDrawing();
  switch (visualizeMode) {
    case 4:  
      for (c=0; c<NCOLS; c++) {
        uint8_t level;
        level = (((int16_t)levelsR[c])*volume)>>8;
        for (r=0;r<level;r++) setPixel(r,c,1);
        for(;r<NROWS;r++)     setPixel(r,c,0);
        level = (((int16_t)levelsR[c])*volume)>>8;
        setPixel(level,c,1);
        
        level = (((int16_t)levelsL[c])*volume)>>8;
        for (r=0;r<level;r++) setPixel(r,c,0xff);
        level = (((int16_t)levelsL[c])*volume)>>8;
        setPixel(level,c,0xff);
      } 
      break;
    case 1:
      for (c=0; c<NCOLS; c++) {
        uint8_t level = (((int16_t)levelsR[c])*volume)>>8;
        for (r=0;r<level;r++) setPixel(r,c,1);
        for(;r<NROWS;r++)     setPixel(r,c,0);
      } 
    break;
    case 2:
      for (c=0; c<NCOLS; c++) {
        uint8_t level = (((int16_t)levelsL[c])*volume)>>8;
        for (r=0;r<level;r++) setPixel(r,c,1);
        for(;r<NROWS;r++)     setPixel(r,c,0);
      } 
    break;
    case 0:
      for (c=0; c<NCOLS; c++) {
        uint8_t level;
        level = (((int16_t)levelsR[c])*volume)>>8;
        for (r=0;r<level;r++) setPixel(r,c,1);
        for(;r<NROWS;r++)     setPixel(r,c,0);
        level = (((int16_t)maxR[c])*volume)>>8;
        setPixel(level,c,0xff);
        level = (((int16_t)minR[c])*volume)>>8;
        setPixel(level,c,0xff);
      } 
    break;
    case 3:
      scrollDown();
      for (c=0; c<NCOLS; c++) {
        int level = levelsR[c];
        level -= 2;
        level = 1<<level;
        if (level>7) level=7;
        if (level<0) level=0;
        setPixel(NROWS-1,c,level);
      } 
      break;
  }
  show();
}


