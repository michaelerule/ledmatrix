#include <avr/interrupt.h>
#include <_FHT.h>  // include the FHT library

#define AVERAGESAMPLES 1
#define leftAudio A0
#define volume 40

#define NOP __asm__("nop\n\t");


volatile uint8_t  current_channel = 0;
volatile uint16_t data_index      = 0;
volatile uint8_t  data_handling   = 0;

// bits are [ADC?][start conversion][auto trigger?][interrupt flag][interrupt enable][three prescaler bits]
// prescaler is five
#define ADCMAGICNUMBER 0b11001100

int levelsL[NCOLS];
int levelsR[NCOLS];
int maxL[NCOLS];
int maxR[NCOLS];
int lowL[NCOLS];
int lowR[NCOLS];
int minL[NCOLS];
int minR[NCOLS];

void init_audio()
{
  //TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX  = 0x40; // use adc0
  DIDR0  = 0x01; // turn off the digital input for adc0
  sei();
  ADCSRA = ADCMAGICNUMBER; 
}


ISR(ADC_vect,ISR_BLOCK)
{
  if (data_handling) return;
  //ADC is ready, grab the data ( LSB first, then high byte )
  int16_t data = 0;
  data  =  (int16_t)ADCL;
  data += ((int16_t)ADCH)*256;
  data -= 1<<9; // form into a signed int ( 10 bit resolution )
  data <<= 6; // form into a 16b signed int
  fht_input[data_index] = data; // put real data into bins   
  if (++data_index>=FHT_N) data_handling = 1; 
  ADCSRA = ADCMAGICNUMBER; //initiate next conversion
}

int check_audio()
{ 
    if (!data_handling) return 0;
    //cli();
    fht_window();   // window the data for better frequency response
    fht_reorder();  // reorder the data before doing the fht
    fht_run();      // process the data in the fht
    fht_mag_log();  // take the output of the fht
    data_handling = 0;
    data_index = 0;
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
    //sei();
    ADCSRA = ADCMAGICNUMBER;
    return 1;
}
