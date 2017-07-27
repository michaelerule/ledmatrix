

/** draws a diagonal for testing */
void diagonal()
{
  for (int r=0;r<NROWS;r++)
  for (int c=0;c<NCOLS;c++)
  if (r==c)
    setPixel(r,c,0xff);
  show();
}

void rowGreyscale()
{
  for (int r=0;r<NROWS;r++)
  for (int c=0;c<NCOLS;c++)
  setColor(r,c,r*(NCOLOR-1)/NROWS+1);
  show();
}

void chex()
{
  for (int r=0;r<NROWS;r++)
  for (int c=0;c<NCOLS;c++)
  if ((r^c)&1)
    setPixel(r,c,0xff);
  show();
}

void allOn()
{ 
   for (int c=0;c<NCOLS;c++) 
     for (int r=0;r<NROWS;r++) //if(c!=6 || r==)
       setPixel(r,c,0xff);
  show();
}
