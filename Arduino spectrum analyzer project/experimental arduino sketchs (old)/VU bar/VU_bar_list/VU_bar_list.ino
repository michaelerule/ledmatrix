
#define REFRESHINTERVAL  255         //!< For compare-to-counter timer interrupt. 
//Assigned to OCR0A. Display driver will trigger every REFRESHINTERVAL tics.

#define PINOFFSET 2
#define NPINS 8

const int mapping[40] = {35,48,7,44,11,24,12,31,13,38,14,45,26,33,27,40,28,47,34,41,2,9,10,17,18,25,26,33,34,41,42,49,3,16,4,23,5,30,6,37};

int lighted[20];

volatile int scani = 0;

void setup() {
  /*
  TIMSK0 = 2; // Timer CompA interupt 1
  TCCR0B = 4; // speed ( 2 works, 4 is slower, prescaler )
  TCCR0A = 2; // CTC mode
  OCR0A  = REFRESHINTERVAL; // period
  sei();
  */
  
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
  int code = mapping[scani];
  int j = code % 7;
  int i = code / 7;
  if (j>=i) j++;
  
  uint16_t scanpins = (uint16_t)1<<(i+PINOFFSET);
  digitalWriteAll(scanpins);
  pinModeAll(0);
  pinModeAll(scanpins|((uint16_t)1<<(j+PINOFFSET)));
  
  scani++;
  if (scani>=40) scani=0;
}

void loop()
{
  scan();
  delay(500);
}
