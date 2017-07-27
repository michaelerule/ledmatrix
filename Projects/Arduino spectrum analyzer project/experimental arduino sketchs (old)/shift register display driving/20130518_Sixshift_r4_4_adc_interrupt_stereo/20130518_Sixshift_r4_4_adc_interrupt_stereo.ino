#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>   // needed for uint8_t
#include <FHT.h>      // include the library

// eventually becomes display lib include
#define LATCHPIN 3 //Pin connected to ST_CP of 74HC595 on PORTD
#define CLOCKPIN 2 //Pin connected to SH_CP of 74HC595
#define DATAPIN 4  //Pin connected to DS    of 74HC595
#define reset  5
#define enable 6
#define NCOLS 18
#define NROWS 20
#define NPIX  (NROWS*NCOLS)
#define NLVL  2

#define display_buffer_type uint32_t

display_buffer_type __db1[NCOLS];
display_buffer_type __db2[NCOLS];
display_buffer_type *drawing_buffer = &__db1[0];
display_buffer_type *display_buffer = &__db2[0];
volatile uint16_t display_scan_index = 0;
void scanDisplay() {
  // one column at a time is displayed
  // each column has 20 pixels, but three shift registers are used so it is 24 pins.
  uint32_t col = (1UL<<display_scan_index)<<(24-NCOLS);
  // we need to grab the row data
  uint32_t row = display_buffer[display_scan_index];
  // oh yeah then this happened ( logic level inversion )
  row = ~row;
  col = ~col;
  // send the pin state out to the shift registers
  // the data line is set at the same time that the clock line is cleared to zero
  // then a rising clock edge is created
  for (uint8_t i=0; i<24;i++) {
    PORTD  = 0b10000&(row>>(i-4));
    PORTD |= 0b100;
  }
  for (uint8_t i=0; i<24;i++) {
    PORTD  = 0b10000&(col>>(i-4));
    PORTD |= 0b100;
  }
  //create a rising edge on the latch pin to show this column
  PORTD = 0b1000;
  if (++display_scan_index>=NCOLS) display_scan_index=0;
}
void setBufferPixel(uint8_t r, uint8_t c, uint8_t v, display_buffer_type *buffer) {
  // right now the display pixels are row-packed so 
  r+=4;
  uint32_t coldata = buffer[c];
  if (v) coldata |=  (1UL<<r);
  else   coldata &=~ (1UL<<r);
  buffer[c] = coldata;
}
#define setPixel(r,c,v) setBufferPixel(r,c,v,drawing_buffer)

uint8_t getBufferPixel(int r, int c, display_buffer_type *buffer) 
{
  return 0;
}

#define getPixel(r,c,v) getBufferPixel(r,c,v,display_buffer)

void show() 
{
  display_buffer_type *temp = display_buffer;
  display_buffer = drawing_buffer;
  drawing_buffer = temp;
}

#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point fht
#define AVERAGESAMPLES 1
#define leftAudio A0
#define sensePot  A2
#define NMODES    4
#define buttonPin 12
#define volume 40

int sampleRange;
int numAvgSamples = 5;
int levelsL[NCOLS];
int levelsR[NCOLS];
int buttonDebounce = 0;
int visualizeMode = 0;

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
  
  uint8_t r,c;
  for (r=0;r<20;r++)
  for (c=0;c<18;c++)
  setPixel(r,c,(r^c)&1);
  show();
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

/*
void loop()
{
    if (data_handling)
    {
      cli();
      fht_window(); // window the data for better frequency response
      fht_reorder(); // reorder the data before doing the fht
      fht_run(); // process the data in the fht
      fht_mag_log(); // take the output of the fht
      setLevels(current_channel?&levelsR[0]:&levelsL[0]); // set the screen state
      data_handling = 0;
      data_index = 0;
      sei();
      current_channel = 1-current_channel;
      if (current_channel) //left
      { 
        ADMUX  = 0x40; // use adc0
        DIDR0  = 0x01; // turn off the digital input for adc0
      } else
      {
        ADMUX  = 0x41; // use adc1
        DIDR0  = 0x02; // turn off the digital input for adc1
      }
  
      ADCSRA = 0b11001101;
    }
 
    int y=0;
    int previous;
    switch (visualizeMode)
    {
      case 0: 
        for(y=0; y < 18; y++)
          displayGrid(y,levelsL[y]*volume>>8,0);//y?levels[y-1]:0);
          //displayGrid(y,(levelsL[y]*volume>>8)+9,20-(levelsR[y]*volume>>8));//y?levels[y-1]:0);
        break;
      case 1: 
        for(y=0; y < 18; y++)
          displayGrid(y,levelsR[y]*volume>>8,0);//y?levels[y-1]:0);
        break;
      case 2: 
        displayGrid(y,(levelsL[0]*volume)>>8,0);
        previous = (levelsL[0]*volume)>>8;
        for(int y=1; y < 18; y++)
        {
          int current = (levelsL[y]*volume)>>8;
          displayGrid(y,current,current==previous?current-1:previous);
          previous = current;
        }
        break;
      case 3: 
        for(int y=0; y < 18; y++)
          displayGrid(y,levelsL[y]*volume>>8,(levelsL[y]*volume>>8)-1);
        break;
    }  
    
    // turn screen off while we get the next audio frame
    digitalWrite(DATAPIN,1);
    for (int i=0;i<24*2;i++) 
    {
      digitalWrite(CLOCKPIN, 0);
      digitalWrite(CLOCKPIN, 1);
    }
    //create a rising edge on the latch pin to show this column
    PORTD &=~ _BV(LATCHPIN);
    PORTD |=  _BV(LATCHPIN);
    
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
*/
const uint8_t fft_monitor[] = {3,7,11,15,19,23,27,31,35,39,43,47,51,55,59,63};
/*
void setLevels(int *levels)
{
  int sampleRange = 200;
  long averageL=0;
  long averageR=0;
  for(int x=2; x <   9; x++) averageL += levels[x];
  for(int y=9; y <= 18; y++) averageR += levels[y];

  levels[17] = averageL/8;
  levels[16] = averageR/8;
  
  uint8_t i,j;
  for (i=0;i<16;i++)
  {
    uint8_t freq = fft_monitor[15-i+1];
    uint16_t average = 0;
    for (j=0;j<AVERAGESAMPLES;j++)
      average += fht_log_out[(15-i)*AVERAGESAMPLES+j];
    average *= log(freq)/4;
    average /= AVERAGESAMPLES;
    levels[i+NCOLS ] = (levels[i+NCOLS  ]*200+56*average)>>8; // smoothing 1
    levels[i+NCOLS*2] = (levels[i+NCOLS*2]*(1024-50)+50*average)>>10; // smoothing 2 (  DC cancellation )
    levels[i] = levels[i+NCOLS];//-levels[i+NCOLS*2];
  }
}
*/

/*
void displayGrid( uint8_t colLevel, uint8_t rowLevelStart, uint8_t rowLevelStop)
{  
  uint32_t col = 1UL<<colLevel;
  uint32_t row = (1UL<<rowLevelStart)-1;
  row ^= (1UL<<rowLevelStop)-1;
  
  row = ~row;
  col = ~col;
  
  col <<= (24-18);
  
  for (uint8_t i=0; i<24;i++)
  {
    PORTD = (row>>(i-4))&0b10000;
    PORTD |= 0b100;
  }
  for (uint8_t i=0; i<24;i++)
  {
    PORTD = (col>>(i-4))&0b10000;
    PORTD |= 0b100;
  }
  
  //create a rising edge on the latch pin to show this column
  PORTD &=~ _BV(LATCHPIN);
  PORTD |=  _BV(LATCHPIN);
}
*/




