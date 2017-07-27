#!/usr/bin/env jython

fname = 'alpha2.png'

from javax.imageio import ImageIO
from java.io import File
from java.awt.image import BufferedImage

fileimage = ImageIO.read(File(fname))

image = BufferedImage(fileimage.width,fileimage.height,BufferedImage.TYPE_INT_RGB);

image.graphics.drawImage(fileimage,0,0,None)

def getcol(image,col):
	return [int(image.getRGB(col,row)!=-1) for row in range(image.height)];

unique = set()

for col in range(image.width):
	c = getcol(image,col)
	unique.add(tuple(c))

print unique
print len(unique)

count = dict([(x,0) for x in unique])

for col in range(image.width):
	c = tuple(getcol(image,col))
	count[c]= count[c]+1;

print sorted(zip(count.values(),count.keys()))


f=open('alphadata.h','w')
f.write('uint8_t alphabet[] PROGMEM = {\n')
clist=[]
for col in range(image.width):
	c = getcol(image,col)
	if c[0]:
		clist.reverse()
		for c in clist:
			encode = len(clist)
			for l in c:
				encode = 2*encode + l
			f.write('0x%02x,'%encode)
		clist = []
	else:
		clist.append(c[1:])

f.write('};\n\n')
f.flush()
f.close()



