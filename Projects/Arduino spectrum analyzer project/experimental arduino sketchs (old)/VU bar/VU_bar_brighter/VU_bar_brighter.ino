


#define REFRESHINTERVAL  255
#define PINOFFSET 2
#define NPINS 8

#define BITDEPTH 4
#define FRAMESIZE NPINS
#define NFRAMES BITDEPTH
#define BUFFERSIZE (NFRAMES*FRAMESIZE)
uint16_t displayMemory[2*BUFFERSIZE];
uint16_t *drawingBuffer = &displayMemory[0];
uint16_t *displayBuffer = &displayMemory[BUFFERSIZE];

int8_t permutation[40] = {
  33,46, 5,42, 9,22, 10, 29, 11,36,12,43,-24,-31,25,38,26,45,-32,39,
   0, 7, 8,15,16,23,-24,-31,-32,39,40,47,  1, 14, 2,21, 3,28,  4,35};

/*
uint8_t permutation[40] = {
  33,46, 5,42, 9,22,10,29,11,36,
  12,43,24,31,25,38,26,45,32,39,
   0, 7, 8,15,16,23,24,31,32,39,
  40,47, 1,14, 2,21, 3,28, 4,35};
*/

void clearDrawing() {
  for (int i=0; i<BUFFERSIZE; i++) drawingBuffer[i] = 0;
}

void init_audio()
{
  DIDR0  = 0b00000111;
  ADCSRB = 0b00000000;
  ADCSRA = 0b11101101;
  ADMUX  = 0b00100000;
}

void init_display()
{
  for (int i=0; i<2*BUFFERSIZE; i++) displayMemory[i] = 0;
  TIMSK0 = 2; // Timer CompA interupt 1
  TCCR0B = 2; // speed ( 2 works, 4 is slower, prescaler )
  TCCR0A = 2; // CTC mode
  OCR0A  = REFRESHINTERVAL; // period
}

void setup() {
  init_audio();
  init_display();
  sei();
}

void show() {
    uint16_t *temp = drawingBuffer;
    drawingBuffer  = displayBuffer;
    displayBuffer  = temp;
}

/**
 * Directly manipulates display data
 * there are 8 lines, but only 7 are 
 * used in each scan since one is used
 * as the (in this case) anode.
 **/
void setPixel(uint8_t i, uint8_t v) {
  uint8_t block = i/(NPINS-1);
  uint8_t bit   = i%(NPINS-1);
  if (bit>=block) bit++;
  bit += PINOFFSET;
  if (v) drawingBuffer[block]|= (uint16_t)1<<bit;
  else   drawingBuffer[block]&=~(uint16_t)1<<bit;
}

uint8_t getPixel(uint8_t i) {
  uint8_t block = i/(NPINS-1);
  uint8_t bit   = i%(NPINS-1);
  if (bit>=block) bit++;
  bit += PINOFFSET;
  return (displayBuffer[block]>>bit)&1;
}

void setBarPixel(uint8_t row, uint8_t column, uint8_t v) {
  int8_t index = permutation[row*20+column];
  if (index>=0) setPixel(index,v);
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

volatile int current_channel = 0;
volatile int audio_max[2];
volatile int audio_min[2];
volatile int audio_mean[2];
volatile int audio_var[2];

ISR(ADC_vect,ISR_BLOCK)
{
  int x = (int)(ADCH-128<<7);
  
  audio_max [current_channel] = max(x,250*audio_max[current_channel]+5*x>>8);
  audio_min [current_channel] = min(x,250*audio_min[current_channel]+5*x>>8);
  audio_mean[current_channel] = 250*audio_mean[current_channel]+5*x>>8;
  x -= audio_mean[current_channel];
  x *= x;
  audio_var[current_channel] = 250*audio_var[current_channel]+5*x>>8;

  current_channel = 1-current_channel;
  ADMUX = 0b00100000 | current_channel;
}

ISR(TIMER0_COMPA_vect, ISR_NOBLOCK) {
  scan();
}

volatile int overscan = 0;
volatile int scani = 0;
void scan() {
  if (overscan) {
    overscan--;
    return;
  }
  
  uint16_t scanpins = displayBuffer[scani];
  uint16_t anode = (uint16_t)1<<(scani+PINOFFSET);
  pinModeAll(0);
  digitalWriteAll(anode);
  pinModeAll(scanpins | anode);
  
  uint8_t w = scanpins>>2;
  w = (w&1)+(w>>1&1);
  w = (w&3)+(w>>2&3);
  w = (w&7)+(w>>3&7);
  w = w>0?w-1:w;
  w = w>0?w-1:w;
  overscan = 1<<w;
  
  if (++scani>=NPINS) scani = 0;
}

void loop()
{
  for (int i=0; i<20; i++)
  {
    clearDrawing();
    /*
      setPixel(permutation[i],1);
      setPixel(permutation[i+20],1);
    */
    for (int j=0; j<=i; j++)
    {
      setBarPixel(0,j,1);
      setBarPixel(1,j,1);
    }
    
    show();
    delay(500);
  }
  
  /*
  clearDrawing();
  for ( int i=0; i<2; i++) {
      uint8_t level = ((audio_mean[i]>>7)-128)*20/256;
      for (int j=0; j<level; j++) 
      {
      setPixel(permutation[20*i+j],1);
      }
  }*/
}
