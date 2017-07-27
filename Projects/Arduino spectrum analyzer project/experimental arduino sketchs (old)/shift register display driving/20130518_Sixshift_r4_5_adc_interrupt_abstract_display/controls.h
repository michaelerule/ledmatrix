#include <arduino.h>

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
