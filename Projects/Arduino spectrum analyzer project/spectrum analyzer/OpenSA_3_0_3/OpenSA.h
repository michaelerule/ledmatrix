/**
 * OpenSA -- an open-source audio visualizer and spectrum analyzer.
 * @file   OpenSA.h
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
 * Open SA supports audio sampling and display.
 * 
 * <hr/>
 * Mode System
 * 
 * The modes operate like different applications that can run on OpenSA
 * Like an arduino sketch, they each have a setup() and a loop() function.
 * They can access the state of Audio and the Display in order to render
 * music visualizations, demos, even games. 
 * 
 * Modes are registered at startup in  
 */
#ifndef opensa_h
#define opensa_h

#include "SpectrumGraphics.h"
#include "SpectrumFHT.h"
#include "SpectrumAudio.h"
#include "SpectrumFont.h"
#include "SpectrumLife.h"

#define NOP __asm__("nop\n\t");
#define delayop(N) { for (uint32_t __delayop_i=(N);__delayop_i;__delayop_i--) NOP }

#define ALPHA    200
#define VOLALPHA 240
#define MAXALPHA 240
#define MINALPHA 240
#define LOWALPHA 240
#define LIRP(ALPHA,A,B) ((A)*(ALPHA)+(256-(ALPHA))*(B)>>8)

#define sensePot  A2
#define buttonPin 7
#define NMODES    11
#define clip(x,a,b) ((x)<(a)?(a):(x)>(b)?(b):(x))

#define NBINS 22
const uint8_t band_edges[NBINS+1] = {2,3,4,5,6,7,8,9,11,13,16,19,23,27,32,38,45,53,64,76,90,107,127};

int levelsL[NCOLS];
int levelsR[NCOLS];
int maxL[NCOLS];
int maxR[NCOLS];
int lowL[NCOLS];
int lowR[NCOLS];

volatile int MODE_RUNNING   = 0;
volatile int INITIALIZED    = 0;
volatile int MODE_OPTION    = 0;
volatile uint8_t VOLUME     = 20;
volatile int buttonDebounce = 0;
volatile int visualizeMode  = 0;

void check_buttons() {
	// read button with debouncing & increment visualizer if triggered
	if (digitalRead(buttonPin)==LOW)
		if (!buttonDebounce)  {
			buttonDebounce = 50;
			visualizeMode = (visualizeMode+1)%NMODES;
			INITIALIZED = 0;
			MODE_RUNNING = 0;
	}
	if (buttonDebounce) buttonDebounce--; 
}

void init_controls() {
	// button is.. input
	pinMode( buttonPin, INPUT );  
	pinMode( buttonPin, INPUT_PULLUP );
	digitalWrite(buttonPin, HIGH);
	display_callback = check_buttons;
}

void init_OpenSA() {
	init_display();
	init_controls();
	init_audio();
	sei();
}

void setLevels(int channel)
{
	if (channel>1) return;

	int *levels  = !channel?&levelsR[0]:&levelsL[0];
	int *maximum = !channel?&maxR[0]:&maxL[0];
	int *low     = !channel?&lowR[0]:&lowL[0];
	
	// these are subject to the automatic gain control which is a bit wrong
	long averageL=0;
	for(int x=2; x < 18; x++) averageL += levelsL[x];
	levels[0] = averageL/8;
	maximum[0] = levels[0]>maximum[0] ? levels[0] : LIRP(MAXALPHA,maximum[0],levels[0]);

	long averageR=0;
	for(int y=2; y < 18; y++) averageR += levelsR[y];  
	levels[1] = averageR/8;
	maximum[1] = levels[1]>maximum[1] ? levels[1] : LIRP(MAXALPHA,maximum[1],levels[1]);  
	
	uint8_t i,j;

	for (i=0;i<NBINS;i++)
	{
		uint8_t k = i+2;
		uint16_t average = 0;
		
		uint8_t naveraged = 0;
		for (uint8_t j=band_edges[i]; j<=band_edges[i+1]; j++)
		{
			average += fht_log_out[j];
			naveraged ++;
		}
		average += naveraged/2;
		average /= naveraged;
		
		low[k] = LIRP(LOWALPHA,low[k],average) ;
		average = average>low[k]? average-low[k]: 0;
		
		average = LIRP(ALPHA,levels[k],average);
		average = average>0? average-1:0;
		if (average>0) average--;
		levels[k] = average;
		
		maximum[k] = average>maximum[k] ? average : LIRP(MAXALPHA,maximum[k],average);
	}
	
}


#include "SpectrumModes.h"

void loop_OpenSA() {
	//MODE_RUNNING = 1;
	//modes[visualizeMode]();
		if (check_audio()) {
				setLevels(current_channel); // set the screen state
			VOLUME = (analogControls[2]>>7)+129;
			leftMode();
		}
}

#endif
/* memory check

FHT memory
8*256 no that's much to large.

display memory
2*NCOLS*BYTESPERCOLUMN*BITDEPTH
2*20*8*3

audio buffer

*/



