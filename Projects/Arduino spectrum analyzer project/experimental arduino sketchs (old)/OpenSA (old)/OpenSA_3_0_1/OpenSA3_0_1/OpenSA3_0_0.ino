#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>   // needed for uint8_t
#include <avr/interrupt.h>

#include "OpenSA.h"
//#include "SpectrumGraphics.h"

void (*modes[])(void) = {
        pongMode,
	leftRightMode1,
	leftRightMode2,
	leftMode,
	rightMode,
	waterfallMode,
        snowCrashMode,
	waveMode,
	AS220Mode,
	gainControlDebugMode,
	gameOfLifeMode,
	NULL
};

void setup() {
  init_display();
  
  allOn();
  //sei();
  //chex();
  rowGreyscale();
  //setPixel(8,0,1);
  //setPixel(1,1,1);
  //show();
  
    //MODE_RUNNING = 1;
    //snowCrashMode();
  //while(1);
  
  init_controls();
  init_audio();
  
  //MODE_RUNNING = 1;
  //while(1) gameOfLifeMode();
  /*
  while (1) 
  if (check_audio()) {
    setLevels(current_channel); 
    //gainControlDebugMode();
    gameOfLifeMode();
  }
  */
  
  //for (int i=0;i<10;i++)
  //while(1)
  //;
}

void loop() {
    MODE_RUNNING = 1;
    modes[visualizeMode]();
    
    if (check_audio()) {
        setLevels(current_channel); // set the screen state
	VOLUME = analogControls[0];//(analogControls[0]>>7)+129;
        //gainControlDebugMode();
        //leftMode();
    }
    
}


