#include <avr/interrupt.h>

#define NOP __asm__("nop\n\t");
#define CLIPTOINT8(x) ((int8_t)((x)<-128?-128:(x)>127?127:(x)))
#define NBANDS 18
#define FILTERDATALEN (NBANDS*4)

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
/** we should set bit 5 of admux so taht the results are left adjusted */

uint8_t levelsL[NCOLS];
uint8_t levelsR[NCOLS];

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
void init_audio()
{
  // set up display interrupt on TIMER1_COMPA_vect
  // Enable OCIE0A Timer 0 Compare Interrupt A by setting bit 1 of TIMSK0
  TIMSK1 = 0b00000010;  
  // Set for Clear Timer on Compare Match so that we start over once the interrupt happens. 
  // Bit 4 of TCCR0B should also be 0 for this mode
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
  TCCR1B = 5;
  // Trigger a display interrupt every OCR1AL ticks
  // You can go higher than 256 by using OCR1AL register
  // Remember to write OCR1AH before OCR1AL if you are using it
  OCR1AL = 4;
  // turn off the digital input for adc0 and adc 1
  DIDR0  = 0x03; 
  // Enable interrupts
  sei();
}

ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
  current_channel ^= 1;
  ADMUX  = ADMUXMAGICNUMBER | current_channel;
  ADCSRA = ADCMAGICNUMBER;
}
const uint8_t LOGTABLE[128] = {0,2,5,8,11,14,16,19,22,25,27,30,33,35,38,41,43,46,48,51,53,56,58,61,63,66,68,70,73,75,78,80,82,84,87,89,91,94,96,98,100,102,
105,107,109,111,113,115,117,120,122,124,126,128,130,132,134,136,138,140,142,144,146,148,150,152,153,155,157,159,161,163,165,167,168,170,172,174,176,177,179,
181,183,185,186,188,190,191,193,195,197,198,200,202,203,205,207,208,210,212,213,215,216,218,220,221,223,224,226,227,229,231,232,234,235,237,238,240,241,243,
244,246,247,249,250,252,253,255};
const int8_t sine_table[256] = {0,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51,54,57,59,62,65,67,70,73,75,78,80,82,85,87,89,91,94,96,98,100,102,103,105,
107,108,110,112,113,114,116,117,118,119,120,121,122,123,123,124,125,125,126,126,126,126,126,127,126,126,126,126,126,125,125,124,123,123,122,121,120,119,118,
117,116,114,113,112,110,108,107,105,103,102,100,98,96,94,91,89,87,85,82,80,78,75,73,70,67,65,62,59,57,54,51,48,45,42,39,36,33,30,27,24,21,18,15,12,9,6,3,0,
-3,-6,-9,-12,-15,-18,-21,-24,-27,-30,-33,-36,-39,-42,-45,-48,-51,-54,-57,-59,-62,-65,-67,-70,-73,-75,-78,-80,-82,-85,-87,-89,-91,-94,-96,-98,-100,-102,-103,
-105,-107,-108,-110,-112,-113,-114,-116,-117,-118,-119,-120,-121,-122,-123,-123,-124,-125,-125,-126,-126,-126,-126,-126,-127,-126,-126,-126,-126,-126,-125,
-125,-124,-123,-123,-122,-121,-120,-119,-118,-117,-116,-114,-113,-112,-110,-108,-107,-105,-103,-102,-100,-98,-96,-94,-91,-89,-87,-85,-82,-80,-78,-75,-73,-70,
-67,-65,-62,-59,-57,-54,-51,-48,-45,-42,-39,-36,-33,-30,-27,-24,-21,-18,-15,-12,-9,-6,-3};
const uint8_t phase_velocities[NBANDS] = {1,2,3,4,5,6,8,10,14,17,22,29,37,47,61,78,100,127};

uint8_t filter_phase[NBANDS*2];
int16_t filter_rpart[NBANDS*2];
int16_t filter_ipart[NBANDS*2];
uint8_t filteroutput[NBANDS*2];
ISR(ADC_vect,ISR_NOBLOCK) 
{
  int16_t impulse = ((int16_t)ADCH - 128)*2;
  
  uint8_t *phase = &filter_phase[NBANDS*current_channel];
  int16_t *rpart = &filter_rpart[NBANDS*current_channel];
  int16_t *ipart = &filter_ipart[NBANDS*current_channel];
  uint8_t *foutp = &filteroutput[NBANDS*current_channel];
  
  for (int i=0; i<NBANDS; i++)
  { 
    uint8_t theta = phase[i];
    int16_t ireal = impulse*sine_table[theta+64];
    int16_t iimag = impulse*sine_table[theta   ];
    int16_t freal = (rpart[i]*200 + ireal)/256;
    int16_t fimag = (ipart[i]*200 + iimag)/256;
    rpart[i] = freal;
    ipart[i] = fimag;
    foutp[i] = freal*freal+fimag*fimag>>8;
    phase[i] = theta + phase_velocities[i];
  }

}
