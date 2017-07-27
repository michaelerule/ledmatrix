#!/usr/bin/env jython

fname = 'numbers.png'

from javax.imageio import ImageIO
from java.io import File
from java.awt.image import BufferedImage

fileimage = ImageIO.read(File(fname))

image = BufferedImage(fileimage.width,fileimage.height,BufferedImage.TYPE_INT_RGB);

image.graphics.drawImage(fileimage,0,0,None)

def getcol(image,col):
	return [int(image.getRGB(col,row)!=-1) for row in range(image.height)];

f=open('alphadata.h','w')
f.write('uint8_t alphabet[] PROGMEM = {\n')

cols = [getcol(image,col) for col in range(image.width)]

while len(cols)>=4:
	char = cols[1:4]
	cols = cols[4:]
	for col in char:
		encode = 0
		col = reversed(col)
		for row in col:
			encode = 2*encode + row
		f.write('0x%02x,'%encode)

f.write('};\n\n')
f.flush()
f.close()

