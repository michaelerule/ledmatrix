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
		delayop(0x10000);
		if (!MODE_RUNNING) break;
	}
}


void waveMode() {
	uint8_t cosine[24] = {15,14,13,12,10,7,5,3,1,0,0,0,0,0,0,0,1,3,5,7,10,12,13,14};
	for (int phase=24; phase>0; phase--) 
	{
		clearDrawing();
		for (int row=0; row<NROWS; row++) {
			for (int col=0; col<NCOLS; col++) {
				int r = row-NROWS/2;
				int c = col-NCOLS/2;
				c=(c*3+1)>>1;
				setPixel(row,col,cosine[(phase+(int)sqrt(c*c+r*r))%24]);
			}
		}
		show();
		phase--;
		if (!MODE_RUNNING) break;
	}
}

void gainControlDebugMode() {
    clearDrawing();
    uint8_t volume = (analogControls[0]);//>>7)+129;
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
	uint32_t R = 0b01001100110101;
	while (MODE_RUNNING) {
		for (int row=0; row<NROWS; row++) 
		for (int col=0; col<NCOLS; col++) 
			setPixel(row,col,COLORS[(((uint8_t)((R^=R>>5^R<<11^R<<1)&255))%NCOLORS)]);
		show();
		delayop(5000);
	}
}

void strobeMode() {
	uint32_t volume = (analogControls[0]);
	for (int row=0; row<NROWS; row++) 
	for (int col=0; col<NCOLS; col++) 
    	setColor(row,col,WHITE);
	show();
	delayop((volume<<8));
	for (int row=0; row<NROWS; row++) 
	for (int col=0; col<NCOLS; col++) 
		setPixel(row,col,BLACK);
	show();
	delayop((volume<<8));
}

void pongMode() {
	int px=NROWS/2, py=2;
	int vx=1, vy=1;
	int av=0;
	int ap=0;
	int bv=0;
	int bp=0;
	
	int score1=0;
	int score2=0;
	while (MODE_RUNNING) 
	{
		clearDrawing();
		//int x = (NROWS-5)*(1024-analogRead(A2))/1024;
		//av = av + (x-ap) >> 1;
		//ap = x;
		for (int i=0; i<5; i++)
			setColor(i+ap,2,WHITE);

		for (int i=0; i<5; i++)
			setColor(i+bp,NCOLS-1,WHITE);
				
		for (int i=0; i<score1; i++)
			setColor(i,0,WHITE);
		for (int i=0; i<score2; i++)
			setColor(i,1,WHITE);
				
		setColor(px,py,WHITE);
		
		if (bp+2<px && bp<NROWS-1) bp++;
		else if (bp+2>px && bp>0) bp--; 
		
		if (ap+2<px && ap<NROWS-1) ap++;
		else if (ap+2>px && ap>0) ap--; 
		
		py += vy;
		px += vx;
		if (px<0 || px>=NROWS) {vx=-vx;px += vx;}
		if (py<2) 
		{
			if (ap>px || (ap+5)<px) {
				px=NROWS/2, py=NCOLS*2/3;
				vx=0, vy=-1;
			}
			else {
				vx += av;
				vy = -vy;
				py += vy;
				score1++;
			}
		}
		if (py>=NCOLS) {
			if (bp>px || (bp+5)<px) {
				px=NROWS/2, py=NCOLS*2/3;
				vx=0, vy=-1;
			}
			else {
				vx += av;
				vy = -vy;
				py += vy;
				score2++;
			}
		}
		
		if (score1>=NROWS || score2>=NROWS) score1=score2=0;

		show();
	}
}

void serialDisplayMode() {


}


#endif
