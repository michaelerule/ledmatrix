#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>   // needed for uint8_t
#include <avr/interrupt.h>

#include "OpenSA.h"

void (*modes[])(void) = {
	//leftRightMode1,
	//leftRightMode2,
	//leftMode,
	//rightMode,
	///waterfallMode,
	//AS220Mode,
	//gainControlDebugMode,
	gameOfLifeMode,
	//waveMode,
	NULL
};

void setup() {
  init_display();
  
  sei();
  rowGreyscale();
  //chex();
  
  //while(1);
  
  init_controls();
  init_audio();
  //MODE_RUNNING = 1;
  //while(1) gameOfLifeMode();
  
  //for (int i=0;i<10;i++)
  //while(1)
  //;
}

void loop() {
    MODE_RUNNING = 1;
    modes[visualizeMode]();
    if (check_audio()) {
        setLevels(current_channel); // set the screen state
	VOLUME = 20;//analogControls[0];//(analogControls[0]>>7)+129;
        //gainControlDebugMode();
        //leftMode();
    }
}


