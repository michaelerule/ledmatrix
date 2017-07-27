#include <OpenSADisplay.h>

#define setColor OpenSADisplay::setColor
#define show     OpenSADisplay::show

/** draws a diagonal for testing */
void diagonal()
{
  for (int r=0;r<NROWS;r++)
  for (int c=0;c<NCOLS;c++)
  if (r==c)
    setColor(r,c,NCOLOR-1);
  show();
}

void rowGreyscale()
{
  for (int r=0;r<NROWS;r++)
  for (int c=0;c<NCOLS;c++)
  setColor(r,c,1+r*(NCOLOR-4)/NROWS);
  show();
}

#define TEXTCOLOR1 (NCOLOR-1)

void welcomeScreen()
{
  for (int r=0;r<NROWS;r++)
  for (int c=0;c<NCOLS;c++)
  setColor(r,c,0);
  
  setColor(NROWS-3,2,TEXTCOLOR1);
  setColor(NROWS-4,2,TEXTCOLOR1);
  setColor(NROWS-5,2,TEXTCOLOR1);
  setColor(NROWS-6,2,TEXTCOLOR1);
  setColor(NROWS-7,2,TEXTCOLOR1);
  setColor(NROWS-8,2,TEXTCOLOR1);
  
  setColor(NROWS-2,3,TEXTCOLOR1);
  setColor(NROWS-3,3,TEXTCOLOR1);
  setColor(NROWS-8,3,TEXTCOLOR1);
  setColor(NROWS-9,3,TEXTCOLOR1);
  
  setColor(NROWS-3,4,TEXTCOLOR1);
  setColor(NROWS-4,4,TEXTCOLOR1);
  setColor(NROWS-5,4,TEXTCOLOR1);
  setColor(NROWS-6,4,TEXTCOLOR1);
  setColor(NROWS-7,4,TEXTCOLOR1);
  setColor(NROWS-8,4,TEXTCOLOR1);
  
  setColor(NROWS-2,6,TEXTCOLOR1);
  setColor(NROWS-3,6,TEXTCOLOR1);
  setColor(NROWS-4,6,TEXTCOLOR1);
  setColor(NROWS-5,6,TEXTCOLOR1);
  setColor(NROWS-6,6,TEXTCOLOR1);
  setColor(NROWS-7,6,TEXTCOLOR1);
  setColor(NROWS-8,6,TEXTCOLOR1);
  setColor(NROWS-9,6,TEXTCOLOR1);
  
  setColor(NROWS-2,7,TEXTCOLOR1);
  setColor(NROWS-3,7,TEXTCOLOR1);
  setColor(NROWS-5,7,TEXTCOLOR1);
  setColor(NROWS-6,7,TEXTCOLOR1);
  
  setColor(NROWS-5,8,TEXTCOLOR1);
  setColor(NROWS-4,8,TEXTCOLOR1);
  setColor(NROWS-3,8,TEXTCOLOR1);
  
  setColor(NROWS-2,10,TEXTCOLOR1);
  setColor(NROWS-3,10,TEXTCOLOR1);
  setColor(NROWS-4,10,TEXTCOLOR1);
  setColor(NROWS-5,10,TEXTCOLOR1);
  setColor(NROWS-6,10,TEXTCOLOR1);
  setColor(NROWS-7,10,TEXTCOLOR1);
  setColor(NROWS-8,10,TEXTCOLOR1);
  setColor(NROWS-9,10,TEXTCOLOR1);
  
  setColor(NROWS-2,11,TEXTCOLOR1);
  setColor(NROWS-5,11,TEXTCOLOR1);
  setColor(NROWS-3,11,TEXTCOLOR1);
  setColor(NROWS-6,11,TEXTCOLOR1);
  setColor(NROWS-8,11,TEXTCOLOR1);
  setColor(NROWS-9,11,TEXTCOLOR1);
  
  setColor(NROWS-2,12,TEXTCOLOR1);
  setColor(NROWS-3,12,TEXTCOLOR1);
  setColor(NROWS-5,12,TEXTCOLOR1);
  setColor(NROWS-6,12,TEXTCOLOR1);
  setColor(NROWS-8,12,TEXTCOLOR1);
  setColor(NROWS-9,12,TEXTCOLOR1);
  
  setColor(NROWS-2,14,TEXTCOLOR1);
  setColor(NROWS-3,14,TEXTCOLOR1);
  setColor(NROWS-4,14,TEXTCOLOR1);
  setColor(NROWS-5,14,TEXTCOLOR1);
  setColor(NROWS-6,14,TEXTCOLOR1);
  setColor(NROWS-7,14,TEXTCOLOR1);
  setColor(NROWS-8,14,TEXTCOLOR1);
  setColor(NROWS-2,15,TEXTCOLOR1);
  setColor(NROWS-3,15,TEXTCOLOR1);
  setColor(NROWS-3,16,TEXTCOLOR1);
  setColor(NROWS-4,16,TEXTCOLOR1);
  setColor(NROWS-5,16,TEXTCOLOR1);
  setColor(NROWS-6,16,TEXTCOLOR1);
  setColor(NROWS-7,16,TEXTCOLOR1);
  setColor(NROWS-8,16,TEXTCOLOR1);
  
  setColor(NROWS-12,2,TEXTCOLOR1);
  setColor(NROWS-13,2,TEXTCOLOR1);
  setColor(NROWS-14,2,TEXTCOLOR1);
  setColor(NROWS-15,2,TEXTCOLOR1);
  setColor(NROWS-18,2,TEXTCOLOR1);
  
  setColor(NROWS-11,3,TEXTCOLOR1);
  setColor(NROWS-12,3,TEXTCOLOR1);
  setColor(NROWS-15,3,TEXTCOLOR1);
  setColor(NROWS-18,3,TEXTCOLOR1);
  setColor(NROWS-19,3,TEXTCOLOR1);
  
  setColor(NROWS-12,4,TEXTCOLOR1);
  setColor(NROWS-15,4,TEXTCOLOR1);
  setColor(NROWS-16,4,TEXTCOLOR1);
  setColor(NROWS-17,4,TEXTCOLOR1);
  setColor(NROWS-18,4,TEXTCOLOR1);
  
  setColor(NROWS-12,6,TEXTCOLOR1);
  setColor(NROWS-13,6,TEXTCOLOR1);
  setColor(NROWS-14,6,TEXTCOLOR1);
  setColor(NROWS-15,6,TEXTCOLOR1);
  setColor(NROWS-16,6,TEXTCOLOR1);
  setColor(NROWS-17,6,TEXTCOLOR1);
  setColor(NROWS-18,6,TEXTCOLOR1);
  setColor(NROWS-19,6,TEXTCOLOR1);
  
  setColor(NROWS-11,7,TEXTCOLOR1);
  setColor(NROWS-12,7,TEXTCOLOR1);
  setColor(NROWS-16,7,TEXTCOLOR1);
  setColor(NROWS-17,7,TEXTCOLOR1);
  
  setColor(NROWS-12,8,TEXTCOLOR1);
  setColor(NROWS-13,8,TEXTCOLOR1);
  setColor(NROWS-14,8,TEXTCOLOR1);
  setColor(NROWS-15,8,TEXTCOLOR1);
  setColor(NROWS-16,8,TEXTCOLOR1);
  setColor(NROWS-17,8,TEXTCOLOR1);
  setColor(NROWS-18,8,TEXTCOLOR1);
  setColor(NROWS-19,8,TEXTCOLOR1);
  
  setColor(NROWS-12,10,TEXTCOLOR1);
  setColor(NROWS-13,10,TEXTCOLOR1);
  setColor(NROWS-14,10,TEXTCOLOR1);
  setColor(NROWS-15,10,TEXTCOLOR1);
  setColor(NROWS-16,10,TEXTCOLOR1);
  setColor(NROWS-17,10,TEXTCOLOR1);
  setColor(NROWS-18,10,TEXTCOLOR1);
  setColor(NROWS-19,10,TEXTCOLOR1);
  
  setColor(NROWS-18,12,TEXTCOLOR1);
  setColor(NROWS-19,12,TEXTCOLOR1);
  
  setColor(NROWS-13,14,TEXTCOLOR1);
  setColor(NROWS-14,14,TEXTCOLOR1);
  setColor(NROWS-15,14,TEXTCOLOR1);
  setColor(NROWS-16,14,TEXTCOLOR1);
  setColor(NROWS-17,14,TEXTCOLOR1);
  setColor(NROWS-18,14,TEXTCOLOR1);
  
  setColor(NROWS-12,15,TEXTCOLOR1);
  setColor(NROWS-13,15,TEXTCOLOR1);
  setColor(NROWS-18,15,TEXTCOLOR1);
  setColor(NROWS-19,15,TEXTCOLOR1);
  
  setColor(NROWS-13,16,TEXTCOLOR1);
  setColor(NROWS-14,16,TEXTCOLOR1);
  setColor(NROWS-15,16,TEXTCOLOR1);
  setColor(NROWS-16,16,TEXTCOLOR1);
  setColor(NROWS-17,16,TEXTCOLOR1);
  setColor(NROWS-18,16,TEXTCOLOR1);
  
  show();
}

void chex()
{
  for (int r=0;r<NROWS;r++)
  for (int c=0;c<NCOLS;c++)
  if ((r^c)&1)
       setColor(r,c,NCOLOR-1);
  show();
}

void allOn()
{ 
   for (int c=0;c<NCOLS;c++) 
     for (int r=0;r<NROWS;r++) //if(c!=6 || r==)
       setColor(r,c,NCOLOR-1);
  show();
}
