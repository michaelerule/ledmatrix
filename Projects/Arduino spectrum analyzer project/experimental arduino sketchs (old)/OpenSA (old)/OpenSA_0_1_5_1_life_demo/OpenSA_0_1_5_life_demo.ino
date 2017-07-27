#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>   // needed for uint8_t
#include <avr/interrupt.h>

#include "SpectrumDisplay.h"
#include "SpectrumLife.h"

void setup() {
  init_display();
  init_life();
}

uint32_t modeCycleCount = 0;

void loop() {
  life();
}


