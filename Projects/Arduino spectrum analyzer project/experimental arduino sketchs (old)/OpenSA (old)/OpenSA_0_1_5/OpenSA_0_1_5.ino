#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>   // needed for uint8_t
#include <avr/interrupt.h>

#include "OpenSA.h"

void setup() {
  init_display();
  /*
  for (int i=18; i>=-14; i--) {
    clearDrawing();
    if (i>2)
      drawString(i,10,"Open");
    else {
      drawString(2,10,"Open");
      drawString(i+16,0,"SA0.1");
    }
    show();
    delayop(0x20000);
  }
  delayop(0x200000);
  for (int i=0; i<18;i++) {
    scrollDown();
    show();
    delayop(0x10000);
  }
 */ 
  init_controls();
  init_audio();
}

uint32_t modeCycleCount = 0;

void loop() {
    if (visualizeMode==15) {
        clearDrawing();
        uint8_t volume = (analogControls[2]>>7)+129;
        for (int row=0; row<NROWS; row++) {
          for (int col=0; col<NCOLS; col++) {
            setColor(row,col,row*NCOLS+col<volume?WHITE:0);
          }
        }
        show();
    }
    else if (visualizeMode==14) {
      uint8_t cosine[16] = {6,6,5,4,3,2,1,0,0,0,1,2,3,4,5,6};
      for (int phase=16; phase>0; phase--) 
      {
        clearDrawing();
        for (int row=0; row<NROWS; row++) {
          for (int col=0; col<NCOLS; col++) {
            int r = row-NROWS/2;
            int c = col-NCOLS/2;
            c=c*5>>1;
            setPixel(row,col,cosine[phase+(int)sqrt(c*c+r*r) &15]+1);
          }
        }
        show();
        phase--;
        delayop(0xa000);
        if (visualizeMode!=14) break; //hack
      }
        modeCycleCount += 40;  
    }
    else if (visualizeMode==13) {
      for (int i=18; i>=-30; i--) {
        clearDrawing();
        drawString(i,6,"AS220 LABS");
        show();
        delayop(0x30000);
        if (visualizeMode!=13) break; //hack
        modeCycleCount = 400;
      }
    }
    if (check_audio()) {
        setLevels(current_channel); // set the screen state
        if (visualizeMode<13) {
          uint8_t volume = (analogControls[2]>>7)+129;
          drawLevels(20);//global_max);
        }
    }
    //if (++modeCycleCount>400) { visualizeMode = (visualizeMode+1)%15; modeCycleCount=0; }
}


