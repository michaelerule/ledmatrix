#!/usr/bin/env jython

fname = 'clear_lower_8.png'
#fname = 'shortandnarrow8.png'

from javax.imageio import ImageIO
from java.io import File
from java.awt.image import BufferedImage

fileimage = ImageIO.read(File(fname))
image = BufferedImage(fileimage.width,fileimage.height,BufferedImage.TYPE_INT_RGB);
image.graphics.drawImage(fileimage,0,0,None)

def getcol(image,col):
	return [int(image.getRGB(col,row)) for row in range(image.height)];

f=open('marquee_font_clear.txt','w')

clist=[]
for col in range(image.width):
	c = getcol(image,col)
	print c[0]
	if not c[0] in [-1,-16777216]:
		for c in clist:
			print c
			intcode = sum([ 2**i if c[i] else 0 for i in range(len(c))])
			f.write('0x%02x\n'%intcode)
		f.write('-\n')
		clist = []
	else:
		clist.append([int(x==-16777216) for x in c])

f.write('\n')
f.flush()
f.close()

