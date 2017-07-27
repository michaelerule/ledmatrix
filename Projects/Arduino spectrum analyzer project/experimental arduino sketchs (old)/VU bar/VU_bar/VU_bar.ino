
#define REFRESHINTERVAL  255         //!< For compare-to-counter timer interrupt. 
//Assigned to OCR0A. Display driver will trigger every REFRESHINTERVAL tics.

#define PINOFFSET 2
#define NPINS 8

const int mapping[40] = {35,48,7,44,11,24,12,31,13,38,14,45,26,33,27,40,28,47,34,41,2,9,10,17,18,25,26,33,34,41,42,49,3,16,4,23,5,30,6,37};

volatile int scani = 0;
volatile int scanj = 0;
volatile uint16_t scanpins = 0;

void setup() {
  /*
  TIMSK0 = 2; // Timer CompA interupt 1
  TCCR0B = 4; // speed ( 2 works, 4 is slower, prescaler )
  TCCR0A = 2; // CTC mode
  OCR0A  = REFRESHINTERVAL; // period
  sei();
  */
  
  scanpins = (uint16_t)1<<(scani+PINOFFSET);
  digitalWriteAll(scanpins);
  
}

void pinModeAll(uint16_t state)
{
  DDRD = state;
  state >>= 8;
  DDRB = state;
}

void digitalWriteAll(uint16_t state)
{
  PORTD = state;
  state >>= 8;
  PORTB = state;
}

ISR(TIMER0_COMPA_vect, ISR_NOBLOCK) {
  scan();
}

void scan() {
  pinModeAll(0);
  
  pinModeAll(scanpins | ((uint16_t)1<<(scanj+PINOFFSET)));
  
  scanj++;
  if (scanj==scani) scanj++;
  if (scanj>=NPINS) {
    if (++scani>=NPINS) scani = 0;
    scanpins = (uint16_t)1<<(scani+PINOFFSET);
    digitalWriteAll(scanpins);
    scanj = 0;
  }
}

void loop()
{
  scan();
  delayMicroseconds(200);
}
