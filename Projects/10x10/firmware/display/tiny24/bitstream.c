
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NBITSTART 2
#define NEXTNBIT (nbits<<=nbits)

uint16_t  data[16];
uint8_t   sparse[40];
uint8_t   nsparse;
uint16_t  sparsehead;

uint8_t readBit(uint8_t *stream,uint16_t index)
{
	return (stream[index>>3]>>(index&7))&1;
}

uint8_t readBits(uint8_t *stream,uint16_t *index,uint8_t nbits)
{
	uint8_t value = 0;
	while (nbits)
	{
		value <<= 1;
		value |= readBit(stream,*index);
		nbits --;
		(*index)++;
	}
	return value;
}

volatile int maxreads=0;

uint8_t nextDelta(uint8_t *stream,uint16_t *index)
{
	uint8_t read;
	uint16_t nbits=NBITSTART;
	uint8_t sum=0;
	int reads =1;
	while (reads<=8)
	{
		read = readBits(stream,index,nbits);
		if (read>0)
			return sum+read;
		sum += (1<<nbits)-1;
		NEXTNBIT;
		reads++;
		if (reads>maxreads) maxreads=reads;
	}
	return 0;
}

uint8_t flipBit(uint8_t *stream,uint16_t index)
{
	stream[index>>3]^=1<<(index&7);
}

uint8_t writeBit(uint8_t *stream,uint16_t index,uint8_t value)
{
	if (readBit(stream,index)!=value) flipBit(stream,index);
}

uint8_t writeBits(uint8_t *stream, uint16_t *index, uint8_t nbits, uint8_t value)
{
	*index += nbits;
	uint16_t i = *index;
	while (nbits)
	{
		i --;
		writeBit(stream,i,value&1);
		value >>= 1;
		nbits --;
	}
}

void writeDelta(uint8_t *stream,uint16_t *index,uint8_t delta)
{
	uint16_t nbits=NBITSTART;
	while (delta)
	{
		uint8_t increment = (1<<nbits)-1;
		if (delta<=increment)
		{
			increment = delta;
			delta = 0;
		}
		else
		{
			delta -= increment;
			increment = 0;
		}
		writeBits(stream,index,nbits,increment);
		NEXTNBIT;
	}
}

void writeValue(uint8_t *stream,uint16_t *index,uint8_t *length,uint8_t *prev,uint8_t value)
{
	writeDelta(stream,index,value-*prev);
	*prev = value;
	(*length)++;
}

void dense2sparse()
{
	uint8_t r,c;
	uint16_t rowdata;
	uint8_t previous = -1;
	sparsehead = 0;
	nsparse = 0;
	for (r=0; r<16; r++)
	{
		rowdata = data[r];
		for (c=0; c<16; c++)
		{
			if (rowdata&1)
			{
				uint8_t location = (r<<4)|c;
				writeValue(sparse,&sparsehead,&nsparse,&previous,location);
			}
			rowdata>>=1;
		}
	}
}

void sparse2list()
{
	uint8_t location = -1;
	uint8_t i;
	sparsehead = 0;
	for (i=0; i<nsparse; i++)
	{
		location += nextDelta(sparse,&sparsehead);
	}
}

int main(char *args,int narg)
{
	int i,maxbits=0;
	srand( (unsigned)time(NULL) );
	while(1){
		for (i=0;i<10;i++) data[i]=rand()&0x3ff;
		for (i=10;i<16;i++) data[i]=0;
		dense2sparse();
		sparse2list();
		if (sparsehead>maxbits) maxbits=sparsehead;
		printf("%d %d %d\n",maxbits,maxbits/8+1,maxreads);
	}
	return 0;
}




