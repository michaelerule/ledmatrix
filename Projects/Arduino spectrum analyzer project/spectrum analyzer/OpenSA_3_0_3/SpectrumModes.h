/**
 * Definition of build in modes
 * @file   SpectrumModes.h
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
 */
#ifndef spectrum_modes_h
#define spectrum_modes_h
#include "DemoModes.h"

void drawChannel(int channel, int yoffset, int nrows, int volume, int mode, int flip) 
{
	int32_t level;
	uint8_t r,c;
	uint8_t color = mode==1?1:WHITE;
	int *levels = channel? &levelsR[0]: &levelsL[0];
	int *maxlev = channel? &maxR   [0]: &maxL   [0];
	for (c=2;c<NCOLS;c++)
	{
		level = levels[c]*nrows/volume;	
		level = clip(level,0,nrows-1);
		if (mode!=1) level++;
		if (mode<2) {
			for (r=0;r<level;r++) setColor(yoffset+flip*r,c,color);
			for (   ;r<nrows;r++) setColor(yoffset+flip*r,c,0);
		}
		if (mode) {
			level = maxlev[c]*nrows/volume;
			level = clip(level,0,nrows-1);
			setColor(yoffset+flip*level,c,WHITE);
		}
	}
}

volatile uint8_t RUNNING = 1;

void leftRightMode1() {
	clearDrawing();
	drawChannel(0,NROWS/2,NROWS/2,VOLUME,MODE_OPTION,1);
	drawChannel(1,      0,NROWS/2,VOLUME,MODE_OPTION,1);
	show();
}

void leftRightMode2() {
	clearDrawing();
	drawChannel(0,NROWS/2  ,NROWS/2,VOLUME,MODE_OPTION,1);
	drawChannel(1,NROWS/2-1,NROWS/2,VOLUME,MODE_OPTION,-1);
	show();
}

void leftMode() {
	clearDrawing();
	drawChannel(0,0,NROWS,VOLUME,MODE_OPTION,1);
	show();
}

void rightMode() {
	clearDrawing();
	drawChannel(1,0,NROWS,VOLUME,MODE_OPTION,1);
	show();
}

void waterfallMode() {
	scrollDown();
	for (int c=2; c<NCOLS; c++) {
		int level = levelsR[c]*NCOLORS*3/VOLUME;
		level = clip(level,0,NCOLORS-1);
		setColor(NROWS-1,c,level);
	}
	show();
}

#endif
