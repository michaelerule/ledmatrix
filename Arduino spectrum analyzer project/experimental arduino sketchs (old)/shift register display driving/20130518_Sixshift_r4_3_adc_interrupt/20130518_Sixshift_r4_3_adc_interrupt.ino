#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point fht

#define AVERAGESAMPLES 7

#define LATCHPIN_low  PORTD &=~ _BV(3)
#define LATCHPIN_HIGH PORTD |=  _BV(3)

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>   // needed for uint8_t
#include <FHT.h>      // include the library

//Pin connected to ST_CP of 74HC595
#define LATCHPIN 3

//Pin connected to SH_CP of 74HC595
#define CLOCKPIN 2

////Pin connected to DS of 74HC595
#define DATAPIN 4

#define reset  5
#define enable 6

#define leftAudio A0
#define sensePot  A2

int sampleRange;
int numAvgSamples = 5;

int levelsL[18];
int levelsR[18];

#define NMODES    3
#define buttonPin 12

int buttonDebounce = 0;
int visualizeMode = 0;

#define volume 24

void setup() {
  // set up shift registers for output
  pinMode(DATAPIN, OUTPUT);
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);

  // set up.. other stuff for output
  pinMode(enable, OUTPUT);
  pinMode(reset, OUTPUT);
  
  // button is.. input
  pinMode( buttonPin, INPUT );  
  pinMode( buttonPin, INPUT_PULLUP );
  digitalWrite(buttonPin, HIGH);  

  //Serial.begin(9600);

  sampleRange = 0;

  digitalWrite(enable, LOW);
  digitalWrite(reset, HIGH);

  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX  = 0x40; // use adc0
  DIDR0  = 0x01; // turn off the digital input for adc0
  
  sei();
  ADCSRA = 0b11001101;
}

volatile uint8_t  current_channel = 0;
volatile uint16_t data_index      = 0;
volatile uint8_t  data_handling   = 0;

ISR(ADC_vect,ISR_BLOCK)
{
  if (!data_handling)
  {
    saveADC();
    ADCSRA = 0b11001101;
  }
}

void saveADC()
{
  //ADC is ready, grab the data ( LSB first, then high byte )
  int16_t data = 0;
  data  =  (int16_t)ADCL;
  data += ((int16_t)ADCH)*256;
  data -= 1<<9; // form into a signed int ( 10 bit resolution )
  data *= 1<<6; // form into a 16b signed int
  fht_input[data_index] = data; // put real data into bins 

  data_index++;
  if (data_index>=FHT_N)
    data_handling = 1;
}

void loop()
{
    if (data_handling)
    {
      cli();
      fht_window(); // window the data for better frequency response
      fht_reorder(); // reorder the data before doing the fht
      fht_run(); // process the data in the fht
      fht_mag_log(); // take the output of the fht
      setLevels(); // set the screen state
      data_handling = 0;
      data_index = 0;
      sei();
      ADCSRA = 0b11001101;
    }
 
    int y=0;
    int previous;
    switch (visualizeMode)
    {
      case 0: 
        for(y=0; y < 18; y++)
          displayGrid(y,levelsL[y]*volume>>8,0);//y?levels[y-1]:0);
        break;
      case 1: 
        displayGrid(y,(levelsL[0]*volume)>>8,0);
        previous = (levelsL[0]*volume)>>8;
        for(int y=1; y < 18; y++)
        {
          int current = (levelsL[y]*volume)>>8;
          displayGrid(y,current,current==previous?current-1:previous);
          previous = current;
        }
        break;
      case 2: 
        for(int y=0; y < 18; y++)
          displayGrid(y,levelsL[y]*volume>>8,(levelsL[y]*volume>>8)-1);
        break;
    }  
    
    // turn screen off while we get the next audio frame
    LATCHPIN_low;
    digitalWrite(DATAPIN,1);
    for (int i=0;i<24*2;i++) 
    {
      digitalWrite(CLOCKPIN, 0);
      digitalWrite(CLOCKPIN, 1);
    }
    LATCHPIN_HIGH;
    
    // read button with debouncing and increment visualizer more if it is triggered
    if (digitalRead(buttonPin)==LOW)
    {
      if (!buttonDebounce) 
      {
        buttonDebounce = 50;
        visualizeMode = (visualizeMode+1)%NMODES;
      }
    }
    if (buttonDebounce) buttonDebounce--;  
    
}

const uint8_t fft_monitor[] = {3,7,11,15,19,23,27,31,35,39,43,47,51,55,59,63};

void setLevels()
{
  int sampleRange = 200;
  long averageL=0;
  long averageR=0;
  for(int x=2; x <   9; x++) averageL += levelsL[x];
  for(int y=9; y <= 18; y++) averageR += levelsL[y];

  levelsL[17] = averageL/8;
  levelsL[16] = averageR/8;
  
  uint8_t i,j;
  for (i=0;i<16;i++)
  {
    uint8_t freq = fft_monitor[15-i+1];
    uint16_t temp = levelsL[i];
    uint16_t average = 0;
    
    for (j=0;j<AVERAGESAMPLES;j++)
      average += fht_log_out[(15-i)*AVERAGESAMPLES+j];
    average /= AVERAGESAMPLES;
    temp = (temp*200+56*average) >> 8;
    levelsL[i] = temp; 
  }
}

void displayGrid( uint8_t colLevel, uint8_t rowLevelStart, uint8_t rowLevelStop)
{  
  uint32_t col = 1UL<<colLevel;
  uint32_t row = (1UL<<rowLevelStart)-1;
  row ^= (1UL<<rowLevelStop)-1;
  
  row = ~row;
  col = ~col;
  
  col <<= (24-18);
  
  LATCHPIN_low;
  for (uint8_t i=0; i<24;i++)
  {
    digitalWrite(CLOCKPIN, 0);
    digitalWrite(DATAPIN , row&1);
    digitalWrite(CLOCKPIN, 1);
    row >>= 1;
  }
  for (uint8_t i=0; i<24;i++)
  {
    digitalWrite(CLOCKPIN, 0);
    digitalWrite(DATAPIN , col&1);
    digitalWrite(CLOCKPIN, 1);
    col >>= 1;
  }
  LATCHPIN_HIGH;
}




