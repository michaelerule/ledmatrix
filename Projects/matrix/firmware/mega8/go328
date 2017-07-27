#./png2int.py
avr-gcc -Os -mmcu=atmega328 ./display.c -o a.o &&\
avr-objcopy -j .text -j .data -O binary a.o a.bin  && du -b ./a.bin &&\
avrdude -c avrispmkII -p m8 -B10 -P /dev/ttyUSB0 -U flash:w:a.bin


#avrdude -c avrispmkII -p m8 -B10 -P /dev/ttyUSB1 -U lfuse:w:0xe1:m
#avrdude -c avrispmkII -p m8 -B10 -P /dev/ttyUSB1 -U lfuse:w:0xe4:m
