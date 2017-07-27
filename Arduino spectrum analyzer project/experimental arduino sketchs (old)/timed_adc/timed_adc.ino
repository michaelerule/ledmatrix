#include <OpenSADisplay.h>
//#include "display.h"
#include "displayTests.h"

/*
#define AVERAGESAMPLES 1
#define leftAudio A0
volatile uint8_t  current_channel = 0;
volatile uint16_t data_index      = 0;
volatile uint8_t  data_handling   = 0;
// bits are [ADC?][start conversion][auto trigger?][interrupt flag][interrupt enable][three prescaler bits]
// prescaler is 0 for "stupid fast"
#define ADCMAGICNUMBER (0b11001000 | 4)
// The 7,6 bits at '01' means 'use Avcc for reference'
// The 5th bit at 1 means 'left align ADC data'
// Bit 4 is unused
// Bits 0..3 select the analog source and are configured later
#define ADMUXMAGICNUMBER 0b01100000
#define ADC_PRESCALER       7
#define ADC_OUTPUT_COMPARE  2
int8_t levelsL[NCOLS];
int8_t levelsR[NCOLS];
int8_t maxL[NCOLS];
int8_t maxR[NCOLS];
int8_t lowL[NCOLS];
int8_t lowR[NCOLS];

/** The audio sampling interrupt routine. 
    
    What frequency range might be needed?
    
    A Piano ranges from 30 to 4200Hz, requiring 8400     (nyquist) or 16800    (fidelity)
    Music may range from 8 to 4400Hz, returing  8800     (nyquist) or 17600    (fidelity)
    Hearing ranges from 20 to 20000Hz requiring 40,000Hz (nyquist) or 80,000Hz (fidelity)
    Historied movies use      12000Hz requiring 24,000Hz (nyquist)
    CDs are sampled at         44,100Hz, sufficient to capture 22,050 (nyquist) or 11,025 (fidelity)
    Transmission audio uses    32,000Hz, sufficient to capture 16,000 (nyquist) or 8,000 (fidelity)
    Historic formats sample at 22,050Hz, sufficient to capture 11,025 (nyquist) or 5,512 (fidelity)
    
    How fast can we sample?
    
    By default, the successive approximation circuitry requires an input clock 
    frequency between 50kHz and 200kHz to get maximum resolution. If a lower resolution than
    10 bits is needed, the input clock frequency to the ADC can be higher than 200kHz to get 
    a higher sample rate.

    The ADC module contains a prescaler, which generates an acceptable ADC clock 
    frequency from any CPU frequency above 100kHz. 
    The prescaling is set by the ADPS bits in ADCSRA.

    A normal conversion takes 13 ADC clock cycles. 
    The first conversion after the ADC is switched on (ADEN in ADCSRA is set)
    takes 25 ADC clock cycles in order to initialize the analog circuitry.
    
    The prescaler map is as follows
    Val  scale  @16MHz  /13          10bitOK?  8bitOK?  Nyquist   
    0    2      8MHz    1,230,769Hz  no        maybe    615,385Hz  above human hearing
    1    2      8MHz    615,385Hz    no        maybe    307,692Hz  above human hearing
    2    4      4MHz    307,692Hz    no        maybe    153,846Hz  above human hearing
    3    8      2MHz    15,3846Hz    no        maybe    76,923Hz   above human hearing
    4    16     1MHz    76,923Hz     no        maybe    38,462Hz   above human hearing
    5    32     500KHz  38,462Hz     no        yes      19,231Hz   human hearing range
    6    64     250KHz  19,230Hz     yes       yes      9,615Hz    low-fi audio sample rate
    7    128    125KHz  9,615Hz      yes       yes      4,808Hz    highest note on a piano
    
    This is just the frequency if we were to leave the ADC in free running mode. 
    In practice, we will need to process and store the output data, which 
    will take time.
    
    To achieve predictable sampling rate, we trigger the ADC using an interrupt on 
    timer 1. Timer 1 is being used in output compare mode. Timer 1 handler blocks 
    interrupts to ensure percise timing ( assuming no other blocking interrupt 
    handlers are installed ). The analog conversion handling routine is slow and
    does not block while it is being executed. However, you still must ensure that
    the analog conversion routine exits switfly and is finished before the next
    sample arrives. 
    
    Relative to the system, the prescalers for timer 1 define sampling rates of
    
    value  prescale  rate      
    1      1         16MHz      above human hearing 
    2      8         2MHz       above human hearing 
    3      64        250KHz     above human hearing
    4      256       62.5KHz    range of human hearing ( two channel nyquist )
    5      1024      15,625Hz   range of piano ( two channel nyquist )
    */
    /*
void init_audio()
{
  // set up audio interrupt on TIMER1_COMPA_vect
  // Enable OCIE1A Timer 1 Compare Interrupt A by setting bit 1 of TIMSK1
  TIMSK1 = 0;//b00000010;  

  // Set for Clear Timer on Compare Match so that we start over once the interrupt happens. 
  // Bit 4 of TCCR1B should also be 0 for this mode
  TCCR1A = 0b00000010;  
  // The low three bits of TCCR1B specify the clock source and are as follows
  // 0 : disabled
  // 1 : system clock ( prescaler 1 )
  // 2 : prescaler 8
  // 3 : prescaler 64
  // 4 : prescaler 256
  // 5 : prescaler 1024
  // 6 : T0 pin falling edge source
  // 7 : T0 pin rising edge source
  TCCR1B = ADC_PRESCALER;
  // Trigger a display interrupt every OCR1AL ticks
  // You can go higher than 256 by using OCR1AL register
  // Remember to write OCR1AH before OCR1AL if you are using it
  OCR1AL = ADC_OUTPUT_COMPARE;
  
  DIDR0  = 0x3;  // turn off the digital input for adc0 and adc 1
  current_channel ^= 1;
  ADMUX  = ADMUXMAGICNUMBER | current_channel;
  ADCSRA = ADCMAGICNUMBER;
  sei();         // Enable interrupts
}

void pause_sampling()
{
  // the opposite of :
  // set up audio interrupt on TIMER1_COMPA_vect
  // Enable OCIE1A Timer 1 Compare Interrupt A by setting bit 1 of TIMSK1
  TIMSK1 = 0;
}

void resume_sampling()
{
  // set up audio interrupt on TIMER1_COMPA_vect
  // Enable OCIE1A Timer 1 Compare Interrupt A by setting bit 1 of TIMSK1
  TIMSK1 = 0b00000010;  
}

ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
  if (data_handling) return;
  ADCSRA = ADCMAGICNUMBER;
}

ISR(ADC_vect,ISR_BLOCK)
{
  if (data_handling) return;
  //ADC is ready, grab the data ( high byte only since we're left aligned )
  
  ADCSRA = ADCMAGICNUMBER;
  
  // if we have collected enough data, proceed to analyze it
  if (++data_index>=FHT_N) {
    pause_sampling();
    data_handling = 1;
  }
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
    //current_channel ^= 1;
    //ADMUX  = ADMUXMAGICNUMBER | current_channel;
    ADCSRA = ADCMAGICNUMBER;
    //resume_sampling();
    //sei();
    return 1;
}
*/

//#include "controls.h"

OpenSADisplay Display = OpenSADisplay();

void setup() {
  Display.init();
  
  for (int r=0;r<10;r++)
  for (int c=0;c<10;c++)
  Display.setPixel(r,c,0xff);
  Display.show();
  
  //diagonal();
  while (1) Display.scan();
  //delayop(1500000); 
  //welcomeScreen(); delayop(1500000); 
  //for (int i=0; i<18; i++) { rowGreyscale(); delayop(10000); welcomeScreen(); delayop(10000); }
  //init_controls();
  //init_audio();
}

void loop() {
  //Display.scan();
  /*
  if (check_audio())
  {
    setLevels(current_channel); // set the screen state
    drawLevels();
  }
  check_buttons(); 
  */
}

const uint8_t fft_monitor[] = {3,7,11,15,19,23,27,31,35,39,43,47,51,55,59,63};

#define ALPHA 230
#define MAXALPHA 240
#define MINALPHA 200
#define LOWALPHA 230
#define LIRP(ALPHA,A,B) ((A)*(ALPHA)+(256-(ALPHA))*(B)>>8)

/*
void setLevels(int channel) {
  int8_t *levels  = channel?&levelsR[0]:&levelsL[0];
  int8_t *maximum = channel?&maxR[0]:&maxL[0];
  int8_t *low     = channel?&lowR[0]:&lowL[0];
  long averageL=0;
  long averageR=0;
  for(int x=2; x <   9; x++) averageL += levels[x];
  for(int y=9; y <= 18; y++) averageR += levels[y];
  levels[0]  = averageL/8;
  maximum[0] = levels[0]>maximum[1] ? levels[0] : LIRP(MAXALPHA,maximum[0],levels[0]);
  levels[1]  = averageR/8;
  maximum[1] = levels[1]>maximum[1] ? levels[1] : LIRP(MAXALPHA,maximum[1],levels[1]);  
  uint8_t i,j;
  for (i=2;i<18;i++) {
    uint8_t freq = fft_monitor[15-(i-2)+1];
    uint16_t average = 0;
    for (j=0;j<AVERAGESAMPLES;j++) {
      uint16_t index = (i-2)*AVERAGESAMPLES+j;
      average += fht_log_out[index];
    }
    average *= log(freq)/3;
    average += AVERAGESAMPLES/2;
    average /= AVERAGESAMPLES;
    low[i] = LIRP(LOWALPHA,low[i],average) ;

    // this subtracts out a running average of the power
    int corrected = average - low[i];
    if (corrected<0) corrected = 0;
    average = corrected;

    levels[i] = LIRP(ALPHA,levels[i],average);
    average = levels[i];
    maximum[i] = average>maximum[i] ? average : LIRP(MAXALPHA,maximum[i],average) ;
  }
}
*/

