#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>   // needed for uint8_t
#include <avr/interrupt.h>

#include "OpenSA.h"
#include "DebugNumerals.h"
//#include "SpectrumGraphics.h"

void (*modes[])(void) = {
        pongMode,
	leftRightMode1,
	leftRightMode2,
        strobeMode,
	gainControlDebugMode,
        snowCrashMode,
	waveMode,
	leftMode,
	rightMode,
	waterfallMode,
	AS220Mode,
	gameOfLifeMode,
	NULL
};


int bareAnalogRead(uint8_t pin)
{
	uint8_t low, high;
	if (pin >= 14) pin -= 14;
	ADMUX = 0x62;//(analog_reference << 6) | (pin & 0x07);
	ADCSRA |= 1<<ADSC;
	while (ADCSRA&(1<<ADSC));
	return ADCH;
}



void setup() {
  init_display();
  init_controls();
  init_audio();
  uint8_t ticker =0;
  allDim();
  //while(1);
  /*
  while(1) {
    clearDrawing();
    uint8_t something = bareAnalogRead(A2);
    drawUInt8(0,2,something);  
    drawUInt8Hex(6,2,ADMUX);  
    drawUInt8Hex(12,2,ADCSRA);  
    show();
  }
  */
  /*
  while(1)
  {
        //uint8_t aa = analogRead(A2)>>2;
        uint8_t aa = bareAnalogRead(A2)>>2;
        clearDrawing();
        drawUInt8(0,13,aa);
        drawUInt8(6,13,ticker++);
        show();
  }
  */
}

void loop() {
    MODE_RUNNING = 1;
    modes[visualizeMode]();
    
    if (check_audio()) {
        setLevels(current_channel); // set the screen state
	VOLUME = analogControls[0];//((int)VOLUME*7 + (int)analogControls[0] + 5)>>3;
        /*
        clearDrawing();

        drawUInt8(0,2,current_channel);
        drawUInt8(6,2,data_handling);
        drawUInt8Hex(12,2,ADCSRA);

        drawUInt8(0,13,VOLUME);
        drawUInt8Hex(6,13,data_index);
        drawUInt8Hex(12,13,ADMUX);

        show();
        */
    }
}


