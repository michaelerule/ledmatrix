#define LOG_OUT 1 // use the log output function
#define FHT_N 128 // set to 256 point fht
#define latchPin_low()  PORTD&=~_BV(3)
#define latchPin_high() PORTD|=_BV(3)

#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>       // needed for uint8_t
#include <FHT.h> // include the library


//Pin connected to ST_CP of 74HC595
int latchPin = 3;
//Pin connected to SH_CP of 74HC595
int clockPin = 2;
////Pin connected to DS of 74HC595
int dataPin = 4;

int reset = 5;
int enable = 6;

const int leftAudio = A0;
const int sensePot = A2;
int sampleRange;
int numAvgSamples = 5;

int levels[18];
int levelsAverage[18][5];

const int buttonPin = 13; 
int buttonState = 0;

//Array to link decimal value with column scanning 1-18
unsigned long column [] = {
  1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072}; 

//Allows decimal value to translate to a row led
unsigned long row [] = {
  1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288}; 

//Fills all the contents of the row up to specified array count
unsigned long rowFill [] = {
  0x80000,0xC0000,0xE0000,0xF0000,0xF8000,0xFC000,0xFE000,0xFF000,0xFF800,0xFFC00,0xFFE00,0xFFF00,0xFFF80,0xFFFC0,0xFFFE0,0xFFFF0,0xFFFF8,0xFFFFC, 0xFFFFE, 0xFFFFF};

unsigned long checker [] = {
0x55555	, 0xAAAAA, 0x55555, 0xAAAAA, 0x55555, 0xAAAAA, 0x55555, 0xAAAAA, 0x55555,0xAAAAA, 0x55555, 0xAAAAA, 0x55555,0xAAAAA, 0x55555, 0xAAAAA, 0x55555,0xAAAAA

};


void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  pinMode(enable, OUTPUT);
  pinMode(reset, OUTPUT);
  pinMode(buttonPin, INPUT);    

  Serial.begin(9600);

  sampleRange = 0;
}

void loop(){

  digitalWrite(enable, LOW);
  digitalWrite(reset, HIGH);

  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode

  /*  for(long y=0; y < 18; y++){       
   displayGrid(column[y], row[random(0,20)]);
   //delay(10);
   }*/

  while(1) { // reduces jitter

    ADMUX = 0x40; // use adc0
    DIDR0 = 0x01; // turn off the digital input for adc0

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
    //_delay_us(1000);
    //setLevels();
    //randomTest();

    //getAverages();

    for(int y=0; y <= 18; y++){      
      //displayGrid(column[y] ,rowFill[levels[y]]);    //filled rows
      //displayGrid(column[y] ,row[20-levels[y]]);      //dot
      //displayGrid(row[19-levels[y]], column[y]);        //sideways'
      displayGrid(column[y] ,checker[y]);      //checker u
      //_delay_us(40);
    }
    // _delay_ms(10);
  }

}


void setLevels(){

  // _delay_ms(2);
  int sampleRange = 200;
  // Serial.write(k);
  long averageL=0;
  long averageR=0;

  for(int x=2; x < 9; x++){       
    averageL=averageL+levels[x];
  }

  for(int y=9; y <= 18; y++){       
    averageR=averageR+levels[y];
  }

  levels[0] = averageL/8;
  levels[1] = averageR/8;

  levels[2] = map(fht_log_out[3], 0,sampleRange,0,20);
  levels[3] = map(fht_log_out[7], 0,sampleRange,0,20);
  levels[4] = map(fht_log_out[11], 0,sampleRange,0,20);
  levels[5] = map(fht_log_out[15], 0,sampleRange,0,20);

  levels[6] = map(fht_log_out[19], 0,sampleRange,0,20);
  levels[7] = map(fht_log_out[23], 0,sampleRange,0,20);
  levels[8] = map(fht_log_out[27], 0,sampleRange,0,20);
  levels[9] = map(fht_log_out[31], 0,sampleRange,0,20);

  levels[10] = map(fht_log_out[35], 0,sampleRange,0,20);
  levels[11] = map(fht_log_out[39], 0,sampleRange,0,20);
  levels[12] = map(fht_log_out[43], 0,sampleRange,0,20);
  levels[13] = map(fht_log_out[47], 0,sampleRange,0,20);

  levels[14] = map(fht_log_out[51], 0,sampleRange,0,20);
  levels[15] = map(fht_log_out[55], 0,sampleRange,0,20);
  levels[16] = map(fht_log_out[59], 0,sampleRange,0,20);
  levels[17] = map(fht_log_out[63], 0,sampleRange,0,20);


  _delay_ms(5);


}

void getAverages(){

  int sampleRange = 200;

  long averageL=0;
  long averageR=0;

  for(int x=2; x < 9; x++){       
    averageL=averageL+levels[x];
  }

  for(int y=9; y <= 18; y++){       
    averageR=averageR+levels[y];
  }

  levels[0] = averageL/8;
  levels[1] = averageR/8;

  for(int x=2; x < 18; x++){     
    for(int y=0; y < numAvgSamples; y++){  
      levelsAverage[x][y]=map(fht_log_out[(4*x)-1], 0,sampleRange,0,20);
    }
    for(int z=0; z < numAvgSamples; z++){  
      levels[x]=levels[x]+levelsAverage[x][z];
    }
    levels[x]=levels[x]/numAvgSamples;

  }


  _delay_ms(5);


}


void displayGrid(long col, long row){

  latchPin_low();

  shiftOut(dataPin, clockPin, MSBFIRST, ~row >> 16); 
  shiftOut(dataPin, clockPin, MSBFIRST, ~row >> 8); 
  shiftOut(dataPin, clockPin, MSBFIRST, ~row); 
  shiftOut(dataPin, clockPin, MSBFIRST, ~col >> 16); 
  shiftOut(dataPin, clockPin, MSBFIRST, ~col >> 8); 
  shiftOut(dataPin, clockPin, MSBFIRST, ~col);  

  latchPin_high();



}


void ramp(){

  for(long y=1; y <= 18; y++){       
    displayGrid(column[y-1] ,rowFill[y-1]);
  }

}

void randomTest(){

  for(int x=2; x < 18; x++){     
    levels[x]=random(0,20);

  }
}

void fireworks(){

  //  int cellX = random(0,20);
  //  int cellY = random(0,20);

  int cellX = 0b100;
  int cellY = 10;

  // levels[cellX] = cellY;
  levels[cellX] = cellY + 0b01;


}


































































