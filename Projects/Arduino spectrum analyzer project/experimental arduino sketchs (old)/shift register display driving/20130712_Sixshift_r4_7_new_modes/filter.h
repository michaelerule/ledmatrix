/** 
 * Online band-pass filtering support for spectrum analizer
 */
 
#define SAMPLERATE 10000
#define NBANDS 18

int16_t impulseL[NBANDS*2];
int16_t impulseR[NBANDS*2];

void handle_input(int16_t x, int16_t *impulseBuffer)
{
  uint8_t i;
  for (i=0; i<NBANDS; i++)
  {
   ; 
  }
}
