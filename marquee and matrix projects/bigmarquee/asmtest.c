
/* 
avr-gcc -Os -mmcu=attiny2313 ./asmtest.c -o asm.o
avr-objcopy -j .text -j .data -O binary asm.o asm.bin
avr-objdump -d asm.o  
*/
#define NOP __asm__("nop\n\t")
#include <avr/io.h>
int main()
{	
    uint16_t j;
	j = 10000;
	while (j--) NOP;
}    	
        	
/*

  34:	81 e1       	ldi	r24, 0x11	; 17 //load a 16 bit integer into two registers
  36:	97 e2       	ldi	r25, 0x27	; 39 
  38:	01 c0       	rjmp	.+2      	; 0x3c <main+0x8> 	2
  3a:	00 00       	nop										1
  3c:	01 97       	sbiw	r24, 0x01	; 1					2
  3e:	e9 f7       	brne	.-6      	; 0x3a <main+0x6>   1 (2 if branching)
  40:	08 95       	ret

anyway looks like it takes five cycles per dumb loop
*/
