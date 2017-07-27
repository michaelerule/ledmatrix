#define LOG_OUT 1 // use the log output function
#define FHT_N 128 // set to 256 point fht

#define LATCHPIN_low  PORTD&=~_BV(3)
#define LATCHPIN_HIGH PORTD|=_BV(3)

#include <util/delay.h>
#include <avr/io.h>
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

uint8_t levels[18];
int levelsAverage[18][5];

#define buttonPin  13 
#define buttonState 0

void setup() {
  // set up shift registers for output
  pinMode(DATAPIN, OUTPUT);
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);

  // set up.. other stuff for output
  pinMode(enable, OUTPUT);
  pinMode(reset, OUTPUT);
  pinMode(buttonPin, INPUT);    

  Serial.begin(9600);

  sampleRange = 0;

  digitalWrite(enable, LOW);
  digitalWrite(reset, HIGH);

  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX  = 0x40; // use adc0
  DIDR0  = 0x01; // turn off the digital input for adc0
}

void loop()
{
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_log(); // take the output of the fht
    sei();
    //getAverages();
    setLevels();
    //uint16_t prev = levels[0]/5;
    for(int y=0; y < 18; y++){      
      displayGrid(y,levels[y]);    //filled rows
      
      //uint16_t current = levels[y]/5;
      //displayGrid(y,current,current);      //dot
      //prev = current;
      
      //displayGrid(row[19-levels[y]], column[y]);        //sideways'
      //displayGrid(column[y] ,checker[y]);      //checker u
      //_delay_us(40);
    }
}


const uint8_t fft_monitor[] = {3,7,11,15,19,23,27,31,35,39,43,47,51,55,59,63};

void setLevels()
{
  int sampleRange = 200;
  long averageL=0;
  long averageR=0;
  for(int x=2; x <   9; x++) averageL += levels[x];
  for(int y=9; y <= 18; y++) averageR += averageR+levels[y];

  levels[17] = averageL/8;
  levels[16] = averageR/8;
  
  uint8_t i;
  for (i=0;i<16;i++)
  {
    uint8_t freq = fft_monitor[15-i];
    uint16_t temp = levels[i];
    temp = (temp*200+56*(uint16_t)fht_log_out[freq])/256;
    levels[i] = temp; 
  }
}

void getAverages()
{
  int  sampleRange = 200;
  long averageL=0;
  long averageR=0;

  for(int x=2; x <   9; x++)  averageL=averageL+levels[x];
  for(int y=9; y <= 18; y++)  averageR=averageR+levels[y];
  levels[0] = averageL/8;
  levels[1] = averageR/8;
  for(int x=2; x < 18; x++)
  {     
    for(int y=0; y < numAvgSamples; y++)
      levelsAverage[x][y]= fht_log_out[(4*x)-1]/10;
    
    for(int z=0; z < numAvgSamples; z++)
      levels[x]=levels[x]+levelsAverage[x][z];
    
    levels[x]=levels[x]/numAvgSamples;
  }
}

void displayGrid( uint32_t col, uint32_t row)//, uint16_t rowstop)
{  
  col = 1UL<<col;
  row = (1UL<<row/5);

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


