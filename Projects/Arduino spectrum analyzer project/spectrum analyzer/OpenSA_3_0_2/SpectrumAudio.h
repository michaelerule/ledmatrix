/**
 * Audio sampling driver for Open Spectrum Analyzer
 * @file   SpectrumAudio.h
 * @date   August, 2013
 * 
 * @copyright Michael Rule (mrule7404@gmail.com) 2013
 * @copyright Joshua  Holt (jholt7532@gmail.com) 2013
 * 
 * This file is part of Open Spectrum Analyzer (OpenSA).
 * 
 * OpenSA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * OpenSA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with OpenSA.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * -----
 * 
 * Channel 0 is for left audio, channel 1 is for right audio. 
 * Channel 2 is hooked up to the potentiometer
 */


/*
Getting this one right is hard. 

The ADC datasheet says that up to 15k sampling is possible
at full resolution. This would only get us up to about 7kHz,
which is a couple octaves short of the full human hearing 
rangs.

In theory one should be able to sample up to 77ksps, which
would give us enough resolution -- at the cost of reduced
accuracy.

We want to use free running mode as close to 40ksps as 
possible with 8 bit resolution, left alignign the result
so that we only have to grab the high half of the ADC output
register. Once the buffer is complete, we want to stop sampling
completely until all the data is processes. 

40ksps
8 bit resolution
left align the result
retrieve sample: ADCH is the only register we need
stop sampling
start sampling

"""
Using the ADC Interrupt Flag as a trigger source makes the ADC
start a new conversion as soonas the ongoing conversion has
finished. The ADC then operates in Free Running mode,
constantly sampling and updating the ADC Data Register. The
first conversion must be started by writing a logical one to
the ADSC bit in ADCSRA. In this mode the ADC will perform
successive conversions independently of whether the ADC
Interrupt Flag, ADIF is cleared or not
"""

"""
By default, the successive approximation circuitry requires an
input clock frequency between 50 kHz and 200 kHz to get maximum
resolution. If a lower resolution than 10 bits is needed, the
input clock frequency to the ADC can be higher than 200 kHz to
get a higher sample rate.
"""

Auto Triggered conversions take 13 cycles of the ADC input
clock. The ADS input clock is prescaled from the system clock.

In free running mode to achieve 40ksps, if each sample takes
13 cycles, we will need a clock rate of 520ksps. This is clearly above the
maximum 200Khz allowable to use the full resolution.

setting the registers

ADMUX 
   7      6       5       4       3       2       1       0
REFS1 | REFS0 | ADLAR |   –   | MUX3  | MUX2  | MUX1  | MUX0

REFS1 and REFS0 set the analog reference. We want to use
the external reference. We use AREF which means these bits are
just set to zero.

ADLAR should be set to left align the result

for the ADMUX bits, we have assigned 
left to channel 0, right to channel 1, and the knob to
channel 2

So, the setting we are looking for is

ADMUX = 0b00100000 | (channel number)


ADCSRA
   7      6       5       4       3       2       1       0
 ADEN   ADSC    ADATE   ADIF    ADIE    ADPS2   ADPS1   ADPS0

ADEN: ADC enable. this should be high whenever we are using 
the ADC.

ADSC: start conversion. write this to 1 to start the first
conversion.

ADATE: auto trigger enable. We may need to set this to 1 and 
set the trigger source to conversion complete flag to effect
free running mode. The trigger source is in status register B.

ADIF: this is set when the ADC is done. This will be 1 while
our interrupt handler is running, and 0 once it is done. We
do not need to manage or read this flag.

ADIE: This should be set to 1 to enable triggering our interrupt
handler.

ADPS 0..2 : These set the prescaler between the system clock
and the ADC clock. They are very important. The arduino is 
clocked at 16MHz. We need to set a prescaler to get this down
as close to 520khz without going under. If exact, we would 
need a prescaler of about 30. 

A prescaler of 16 gets us to 38461.5384615 Hz
A prescaler of 32 gets us to 19230.7692308 Hz

The code for a prescaler of 16 is 0b100, for 32 is 0b101

So, we should set ADCSRA to
0b11101100 To start a conversion

Possible ways of pausing conversion:
0b01101100 To halt the ADC
0b10101100 To halt free running
0b11100100 To detach interrupt

ADCSRB

   7      6       5       4       3       2       1       0
   –    ACME      –       –       –     ADTS2   ADTS1   ADTS0

( keep all reserved bits 0 )

ADTS 0..2 : trigger source. We need to set this up for free
running. Actually, we just need to leave this to 0 for free 
running.

ACME: something to do with comparators, leave 0

DIDR0 -- digital data disable register

The lower 6 bits can be set to 1 to block the digital function
of the corresponding ADC pin. We currently use 0..2 for ADC 
and should set this register accordingly.

DIDR0 = 0b00000111

Conclusion: 

to initialize

DIDR0  = 0b00000111;
ADCSRB = 0b00000000;
ADCSRA = 0b10101100;
ADMUX  = 0b00100000;

to begin free running sampling of a channel

ADCSRA = 0b11101100
ADMUX  = 0b00100000 | (channel number)

to stop free running sampling of a channel.
This will stop free-running sampling, although the current free-run 
sample will still happen. I am detatching the interrupt so hopefully it
won't be registered.?

ACSRA = 0b10100100


The mode which samples the full audio spectrum is a bit laggy, on account
of all the processing. A narrower, more reactive mode is appropriate for
most music visualization. The highest note on the piano is 4.186kHz, for
a nyquist of 4.186*2 = 8.372 ksps. If it takes 13 ADC clock cycles to
sample in free running mode, we would need a clock rate of 108.836lHz. 

However, a 256 point FFT can only resolve 128 frequencies -- so, only 7 
octaves. A compromise would give us an upper frequence of about 3873Hz, 
thus we would need the ADC clock to be about 100.7kHz. With a 16MHz clock
that would require a prescaler of about 158 -- but only powers of two
are available.

A prescaler of 128 give us 38Hz to 4.8kHz, which I suppose will have to
do. We should make a music reactive mode that just spans this range.


*/

#ifndef spectrum_audio_h
#define spectrum_audio_h

#include <avr/interrupt.h>
#include <SpectrumFHT.h>


int analogControls[4]; //!< single scalar values read from analog inputs not used for audio ( controls ). Currently only 2 is in use.


/**
 * Initializes audio configuration. 
 * This does not enable interrupts. init_OpenSA calles this initializer
 * and then calls sei() after other initialization is done. If you wish to
 * use this file in isolation, you should manually enable interrupts when
 * you are ready for audio sampling to start.
 */
void init_audio()
{
  DIDR0  = 0b00000111;
  ADCSRB = 0b00000000;
  ADCSRA = 0b11101101;
  ADMUX  = 0b00100000;
}

/**
 * Audio sampling interrupt handling routine
 */

volatile uint8_t  current_channel = 0; //!< Audio input line currently being sampled
volatile uint8_t  data_handling   = 0; //!< Boolean flag: true when sampling is paused for processing

uint16_t data_index_hf = 0;
uint16_t data_index_lf = 0;
uint8_t  audio_input_lf[256]; 
uint8_t  fht_upper_log_out[128];
uint16_t analog_accumulator = 0;
uint8_t  accumulator_index  = 0;

uint8_t  audio_input_hf[256];
//uint8_t * const audio_input_hf = (uint8_t *)(&fht_input[128]);

ISR(ADC_vect,ISR_BLOCK)
{
  uint8_t temp = ADCH;
  analog_accumulator += temp;
  if (++accumulator_index==8) 
  {
    audio_input_lf[data_index_lf] = analog_accumulator >> 3;
    accumulator_index = analog_accumulator = 0;
    data_index_lf++;
  }
  if (data_index_lf>240)
  {
    //*(uint8_t *)(&fht_input[data_index_hf]) = temp;
    //*((uint8_t *)(&fht_input[data_index_hf])+1) = temp>>1;
    audio_input_hf[data_index_hf] = temp;
    //fht_input[data_index_hf] = (int16_t)(temp<<6);//temp;
    //fht_input[data_index_hf] = (int16_t)(temp-128<<7);
    data_index_hf++;
  }
  if (data_index_hf>=256 || current_channel>1) 
  {
    ADCSRA = 0b10000100; //STOP!
    data_handling = 1; 
  }
}

/** 
 * Audio processing routine. Performs FFT and stores the output for future
 * use. Also switches channels.
 */
int check_audio()
{ 
    if (!data_handling) return 0;
    if (current_channel<2) 
    {
    for (uint16_t i=0; i<256; i++)
      fht_input[i] = (int16_t)(audio_input_hf[i]-128<<7);

    //for (uint16_t i=0; i<256; i++)
    //  fht_input[i] = (int16_t)((*(uint8_t *)(&fht_input[i]))-128<<7);
    
      // only worry about FHT for audio channels, not controls
    //fht_window();   // window the data for better frequency response
    fht_reorder();  // reorder the data before doing the fht
    fht_run();      // process the data in the fht
    fht_mag_log();  // take the output of the fht
    
    for (uint16_t i=0; i<128; i++)
      fht_upper_log_out[i] = fht_log_out[i];
    
    for (uint16_t i=0; i<256; i++)
      fht_input[i] = (int16_t)(audio_input_lf[i]-128<<7);
    
      // only worry about FHT for audio channels, not controls
    //fht_window();   // window the data for better frequency response
    fht_reorder();  // reorder the data before doing the fht
    fht_run();      // process the data in the fht
    fht_mag_log();  // take the output of the fht
  }
  else {
    analogControls[current_channel-2] = audio_input_lf[data_index_lf-1];
  }
  data_index_lf = 0;
  data_index_hf = 0;
    if (++current_channel>2) current_channel=0;
  ADMUX = 0b00100000 | current_channel;
    if (current_channel<2)
    {
      // free run mode for audio sampling
    ADCSRA = 0b11101100;
  } else {
    // single conversion mode for control readout
    ADCSRA = 0b10001100;
    ADCSRA = 0b11001100;
  }
  data_handling = 0;
    return 1;
}



#endif


