#!/usr/bin/env jython

'''
Program for generating bit packed ascii text
'''

from java.lang      import *
from java.lang.Math import *
from java.awt       import *
from javax.swing    import *
from java.awt.image import *
from java.awt.color import *
from java.awt.font  import *

ASCII  = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
fonts  = ['Liberation Mono','Liberation Sans','Liberation Sans Narrow','Liberation Serif']
styles = [Font.PLAIN,Font.BOLD,Font.ITALIC,Font.BOLD|Font.ITALIC]
sizes  = range(7,36)
stylenames = ['plain','bold','italic','bold_italic']

def getCol(img,i):
	return [img.getRGB(i,row) for row in range(img.height)]

def getColBytes(img,i,nbytes):
	data   = getCol(img,i)
	data   = ''.join([str(1^x&1) for x in data])
	length = nbytes*8
	data   = data + '0'*(length-len(data))
	data   = ['0b'+data[i*8:(i+1)*8] for i in range(nbytes)]
	return data

def getGlyphBytes(img,nbytes):
	data = [getColBytes(img,j,nbytes) for j in range(img.width)]
	def isBlank(bytes):
		return all([x=='0b00000000' for x in bytes])
	if all([isBlank(x) for x in data]):
		return ''
	while isBlank(data[0]):
		data = data[1:]
	while isBlank(data[-1]):
		data = data[:-1]
	return data

def getFontMetrics(g,font):
	g.font=font
	top = None
	bot = None
	for c in ASCII:
		layout = TextLayout(c, font, g.fontRenderContext);
		y = layout.bounds.y
		h = layout.bounds.height
		t = y+h
		if not top or y<top:
			top=y
		if not bot or t>bot:
			bot=t
	height = int(ceil(bot-top))
	nbytes = int(ceil(height/8.))
	return height,top,nbytes

def getFontBytes(font):
	height,top,nbytes = getFontMetrics(BufferedImage(100,100,BufferedImage.TYPE_INT_RGB).createGraphics(),font)
	img = BufferedImage(80,height,BufferedImage.TYPE_INT_RGB)
	g = img.createGraphics()
	g.font = font
	result = []
	for c in ASCII:
		g.color = Color.WHITE
		g.fillRect(0,0,img.width,img.height)
		g.color = Color.BLACK
		g.drawString(c,0,-top)
		result.append(getGlyphBytes(img,nbytes))
	return nbytes,height,top,result

def entropy(fontdata):
	counts = dict()
	total = 0
	for glyph in fontdata:
		for col in glyph:
			for byte in col:
				if not byte in counts:
					counts[byte]=0
				counts[byte] += 1
				total += 1
	entropy = 0
	ln2 = -1.0/log(2)
	for count in counts.values():
		p = float(count)/total
		entropy += p*log(p)*ln2
	return entropy*100/8.0


for fontname in fonts:
	for s,style in enumerate(styles):
		for size in sizes:
			font = Font(fontname,style,size)
			includename = fontname.lower().replace(' ','_')+'_'+stylenames[s]+'_'+str(size)
			nbytes,height,top,data = getFontBytes(font)
			data = reversed(data)
			datalen = nbytes*sum(map(len,data))
			glyphwidth = map(len,data)
			glyphstart = [sum(glyphwidth[:i]) for i in range(len(data))]
			outfile = open('./fonts/%s.h'%includename,'w')
			outfile.write('#ifndef %s_H\n'%includename)
			outfile.write('#define %s_H\n'%includename)
			outfile.write('#include <stdint.h>\n')
			outfile.write('// font: %s\n'%fontname)
			outfile.write('// style: %s\n'%stylenames[s])
			outfile.write('// size: %s\n'%size)
			outfile.write('// entropy: '+'%s'%entropy(data)+' %\n')
			outfile.write('#define  font_nbytes  %d\n'%nbytes)
			outfile.write('#define  font_nglyphs %d\n'%len(data))
			outfile.write('#define  font_height  %d\n'%height)
			outfile.write('#define  font_top     %d\n'%top)
			outfile.write('PROGMEM prog_uint16_t font_glyphStart[%s] = {\n'%len(data))
			outfile.write(','.join(map(str,glyphstart)))
			outfile.write('};\n')
			outfile.write('PROGMEM prog_uint16_t font_glyphWidth[%s] = {\n'%len(data))
			outfile.write(','.join(map(str,glyphwidth)))
			outfile.write('};\n')
			outfile.write('PROGMEM prog_uint8_t  font_data[%s] = {\n'%datalen)
			outfile.write(',\n\n'.join([',\n'.join([','.join(col) for col in glyph]) for glyph in data]))
			outfile.write('};\n')
			outfile.write('#endif\n')
			outfile.flush()
			outfile.close()

		

