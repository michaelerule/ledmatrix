#!/usr/bin/env jython
# -*- coding: utf-8 -*-

"""
generate.py

This script converts an image of a bitmap font into a compact representation
suitable for use on AVR microcontrollers.

The image format:

Characters are of a fixed height. The image height equals the character 
height. Characters may have any width. Each character is separated by a
vertical column of white 0xffffff, with a magenta 0xff00ff pixel at the
top. Characters are drawn on a white 0xffffff background with a black
0x000000 foreground. The separating whitespace is not included in the 
character. 

The mapping between glyphs and characters must be manually specified in
the charmap string. Only 7-bit ASCII is supported, control characters and
8-bit ASCII are undefined behavior.

Oh, chars can only be 8px high max, since we pack them in bytes

"""


font = '8.narrow.png'

imageOrder = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890 !.,;:'\"[]()#@$%^&*=+<>|/\\"
ASCIIOrder = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~"


from javax.imageio  import ImageIO
from java.io        import File
from java.awt.image import BufferedImage

# get the font image and convert it to a buffered image so we can directly
# access the pixel data. This also guarantees that pixel data will be in 
# the intRGB format
fontImage = ImageIO.read(File(font))
image     = BufferedImage(fontImage.width,fontImage.height,BufferedImage.TYPE_INT_RGB);
image.graphics.drawImage(fontImage,0,0,None)

# returna the int RGB packed column data in a list
def getcol(image,col):
	return [int(image.getRGB(col,row)) for row in range(image.height)];

# scan the image data and output it as a C header file that we can include

allChars = []
charData = []
for col in range(image.width):
	c = getcol(image,col)
	if not c[0] in [-1,-16777216]:
		# if the color is not black or white, dump the character contents
		intCodes = []
		for c in charData:
			intcode = sum([ 2**i if c[i] else 0 for i in range(len(c))])
			intCodes.append('%d'%intcode)
		allChars.append(intCodes)
		charData = []
	else:
		charData.append([int(x==-16777216) for x in c])

charWidths = map(len,allChars)
charOffset = [sum(charWidths[:i]) for i in range(len(charWidths))]

print '#include <avr/pgmspace.h>'
print 'PROGMEM prog_uint8_t charWidths[]={'+','.join(map(str,charWidths))+'};'
print 'PROGMEM prog_uint8_t charOffset[]={'+','.join(map(str,charOffset))+'};'
print 'PROGMEM prog_uint8_t charData[]={'+','.join([x for ch in allChars for x in ch])+'};'

ASCIIMAP = [imageOrder.index(ch) if ch in imageOrder else 255 for ch in ASCIIOrder]

print 'PROGMEM prog_uint8_t ASCIIMap[]={'+','.join(map(str,ASCIIMAP))+'};'





