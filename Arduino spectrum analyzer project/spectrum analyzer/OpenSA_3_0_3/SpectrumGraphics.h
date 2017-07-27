/**
 * Graphics API for Open Spectrum Analyzer.
 * @file   SpectrumGraphics.h
 * @date   August, 2013
 */

#ifndef spectrum_graphics_h
#define spectrum_graphics_h

#include "SpectrumDisplay.h"

#define NCOLORS 6
const uint8_t COLORS[NCOLORS] = {0, 1, 2, 3, 5, 7};

//#define NCOLORS 10           //!< number of gamma corrected display colors
//const uint8_t COLORS[NCOLORS] = {0, 1, 2, 3, 5, 6, 7, 9, 12, 15};  //!< gamma correction map 
//#define NCOLORS 20           //!< number of gamma corrected display colors
//const uint8_t COLORS[NCOLORS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 21, 24, 27, 31};

#define WHITE   (NCOLORS-1) //!< brigtest color
#define GREY    (NCOLORS/2) //!< intermediate brightness color
#define BLACK   0           //!< off


/**
 * Sets pixel data at a given row, column ( no bounds check is performed ).
 * Results are drawn to the drawing buffer and are not immediately 
 * available on screen. Call show() to make the results of drawing visible.
 * @param r row between 0 and NROWS-1
 * @param c column between 0 and NCOLS-1
 * @param v pixel data between 0 and 2^BITDEPTH-1
 */
void setPixel(uint8_t r,uint8_t c,uint8_t v) {
	setBufferPixel(r,c,v,drawing_buffer);
}

/**
 * Retrieves pixel data at a given row, column ( Caution: no bounds 
 * checking is performed ).
 * @param r row
 * @param c column
 * @retval a uint8_t representing currently displayed (r,c)
 */
uint8_t getPixel(uint8_t r,uint8_t c) {
	return getBufferPixel(r,c,display_buffer);
}

/**
 * Sets pixel value with gamma correction. Bounds checking is performed.
 * color values are mapped to display brightness values using the COLORS
 * array. The number of supported colors is defined by NCOLORS. Results 
 * are drawn to the drawing buffer and are not immediately available on
 * screen. Call show() to make the results of drawing available on screen.
 * @param r row
 * @param c column
 * @param v value between 0 and NCOLORS-1
 */
void setColor(int16_t r,int16_t c,int8_t v) {
	if (r<0 || c<0 || r>=NROWS || c>=NCOLS) return;
	v = v<0?0:v>=NCOLORS?NCOLORS-1:v;
	setBufferPixel(r,c,COLORS[v],drawing_buffer);
}


/**
 * Flips the drawing and display buffers, required to show the results of
 * drawing commands.
 */
void show() {
	please_flip_buffers=1;
	while(please_flip_buffers);
}

/**
 * Clears ( sets to black ) the drawing buffer, Remember to call show() to
 * see the results on screen.
 */
void clearDrawing() {
	initializeDisplayBuffer(drawing_buffer);
}

/**
 * Scrolls the current visible screen down one pixel, call show() to see 
 * the results on screen.
 */
void scrollDown() {
	int c,r=0;
	for (;r<NROWS-1;r++)
	  for (c=0; c<NCOLS; c++) 
	    setPixel(r,c,getPixel(r+1,c));
	for (c=0; c<NCOLS; c++) 
		  setColor(NROWS-1,c,0); 
}

/**
 * Scrolls the current visible screen up one pixel, call show() to see the
 * results on screen.
 */
void scrollUp() {
	int r;
	for (r=NROWS-1;r>0;r)
	  for (int c=0; c<NCOLS; c++) 
	    setPixel(r,c,getPixel(r-1,c));
}


/**
 * Scrolls the current visible screen left one pixel, call show() to see the
 * results on screen.
 */
void scrollLeft() {
	int r,c;
    for (c=0; c<NCOLS-1; c++) 
		for (r=0;r>NROWS;r)
			setPixel(r,c,getPixel(r,c+1));
}

/**
 * Scrolls the current visible screen right one pixel, call show() to see the
 * results on screen.
 */
void scrollRight() {
	int c=0,r=0;
	for (c=0;c<NCOLS-1;c++)
	  	for (r=0; r<NROWS; r++) 
		    setPixel(r,c,getPixel(r,c+1));
	for (r=0; r<NROWS; r++) 
		  setColor(r,NCOLS-1,0); 
}


/**
 * A display test pattern that lights all pixels along the diagonal,
 * Results will automatically be shown on screen.
 */
void diagonal() {
	for (int r=0;r<NROWS;r++)
	for (int c=0;c<NCOLS;c++)
	if (r==c) setPixel(r,c,r==c?WHITE:BLACK);
	show();
}


/**
 * A display test patterns demonstrating brightness levels,
 * Results will automatically be shown on screen.
 */
void rowGreyscale() {
int cc = 0;
	for (int r=0;r<NROWS;r++)
	for (int c=0;c<NCOLS;c++) 
	setPixel(r,c,COLORS[r%NCOLORS]);
	show();
}

/**
 * A display test pattern lighting every other pixel in a checkerboard 
 * pattern,
 * Results will automatically be shown on screen.
 */
void chex() {
	for (int r=0;r<NROWS;r++)
	for (int c=0;c<NCOLS;c++)
	if ((r^c)&1)
	  setPixel(r,c,WHITE);
	show();
}

/**
 * A display test pattern that sets all pixels to their full brightness,
 * Results will automatically be shown on screen.  
 */
void allOn() { 
	 for (int c=0;c<NCOLS;c++) 
	   for (int r=0;r<NROWS;r++)
	     setPixel(r,c,WHITE);
	show();
}

void allDim() { 
	 for (int c=0;c<NCOLS;c++) 
	   for (int r=0;r<NROWS;r++)
	     setPixel(r,c,1);
	show();
}

#endif
