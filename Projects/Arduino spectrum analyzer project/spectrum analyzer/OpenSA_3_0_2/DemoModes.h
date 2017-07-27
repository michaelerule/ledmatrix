/**
 * Definition of demonstration in modes
 * @file   DemoModes.h
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
#ifndef demo_modes_h
#define demo_modes_h


void AS220Mode() {
	for (int i=18; i>=-120; i--) {
		clearDrawing();
		drawString(i,-2,"AS220 LABS");
		show();
		delayop(0x30000);
		if (!MODE_RUNNING) break;
	}
}


void waveMode() {
	uint8_t cosine[10] = {3,3,2,1,0,0,0,0,1,2};
	for (int phase=10; phase>0; phase--) 
	{
		clearDrawing();
		for (int row=0; row<NROWS; row++) {
			for (int col=0; col<NCOLS; col++) {
				int r = row-NROWS/2;
				int c = col-NCOLS/2;
				c=(c*3+1)>>1;
				setPixel(row,col,cosine[(phase+(int)sqrt(c*c+r*r))%10]);
			}
		}
		show();
		phase--;
		if (!MODE_RUNNING) break;
	}
}

void gainControlDebugMode() {
    clearDrawing();
    uint8_t volume = (analogControls[2]>>7)+129;
    for (int row=0; row<NROWS; row++) {
      for (int col=0; col<NCOLS; col++) {
        setColor(row,col,row*NCOLS+col<volume?WHITE:0);
      }
    }
    show();
}

void gameOfLifeMode() {
	init_life();
	while (MODE_RUNNING) life();
}

void snowCrashMode() {
	uint32_t rngstate = 0b01001100110101;
	while (MODE_RUNNING) {
		for (int row=0; row<NROWS; row++) 
		for (int col=0; col<NCOLS; col++) 
			setPixel(row,col,(((uint8_t)((rngstate^=rngstate>>5^rngstate<<11^rngstate<<1)&0xff))&7)<1);
		show();
		delayop(5000);
	}
}

void pongMode() {
	int vx=0, vy=0;
	int px=0, py=0;
	int avy=0;
	int apy=0;
	int bvy=0;
	int bpy=0;
	
	while (MODE_RUNNING) 
	{
		;
	}
}

void serialDisplayMode() {


}


#endif
