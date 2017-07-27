#!/usr/bin/env python

'''
outputs a series of commands to build a multiplexd array in eagle
'''

partname = 'TL1105'
#partname = 'B3F-10XX'
rowspace = 0.5
switchoffset = -0.02
diodeoffset = 0.115
#diodeoffset = 0.5*0.35+switchoffset
diode = 'DO34-5@diode'
LED = 'LED3MM'
cathodepinname = 'C'

cols = 4
rows = cols

def add(part,name,x,y,rotate=0):
	print "ADD '%s' R%d %s (%f %f);"%(part,rotate,name,x,y)

def connect(name,part1,pad1,part2,pad2):
	print "SIGNAL %s %s %s %s %s;"%(name,part1,pad1,part2,pad2)

enumeration = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'

'''
boardsize = float(3.2)*3/4
rowspace = boardsize/rows
colspace = boardsize/cols
'''

colspace = rowspace

indicatiors = 1
holes = 0
charlieplex = 0
addheaders = 0
addfineheaders = 0
headerpitch = 0.1

addchip = 1
#chipname = 'SOCKET-14' #'SO14'
#chippins = [2,3,4,5,6,7,8,9,13,12,10]

chipname = 'SO14@maxim'
chippins = [2,3,4,5,6,7,8,9,13,12,10]

boardwidth=rowspace*rows
boardheight=colspace*cols

if (boardheight>3.2 or boardwidth>4):
	print "TOO BIG"
	exit(0)

def getpartname(r,c):
	return '%s%s'%(enumeration[r],enumeration[c])

if indicatiors:
	if holes:
		for r in range(rows):
			for c in range(cols):
				add(partname,getpartname(r,c),switchoffset+c*colspace+0.5*colspace,switchoffset+r*rowspace+0.5*rowspace,rotate=45)
	else:
		for r in range(rows):
			for c in range(cols):
				add(partname,getpartname(r,c),switchoffset+c*colspace+0.5*colspace,switchoffset+r*rowspace+0.5*rowspace,rotate=45)
else:
	for r in range(rows):
		for c in range(cols):
			add(partname,getpartname(r,c),switchoffset+c*colspace+0.5*colspace,r*rowspace+0.5*rowspace)

if not charlieplex:
	if indicatiors:
		for r in range(rows):
			for c in range(cols):
				add(LED,'i'+getpartname(r,c),diodeoffset+c*colspace+0.5*colspace,diodeoffset+r*rowspace+0.5*rowspace,rotate=-135)
		for r in range(rows):
			for c in range(cols-1):
				connect('r%d'%r,'i'+getpartname(r,c),'K','i'+getpartname(r,c+1),'K')
		for c in range(cols):
			for r in range(rows-1):
				connect('c%d'%c,'i'+getpartname(r,c),'A','i'+getpartname(r+1,c),'A')

	for r in range(rows):
		for c in range(cols-1):
			connect('r%d'%r,getpartname(r,c),'1',getpartname(r,c+1),'1')
	for c in range(cols):
		for r in range(rows-1):
			connect('c%d'%c,getpartname(r,c),'2',getpartname(r+1,c),'2')
	if addheaders:
		print "add 1x%02d/90 'rh1' R90 (%f %f);"%(rows,colspace/2.0,rowspace*rows/2)
		print "add 1x%02d/90 'rh2' R270 (%f %f);"%(rows,colspace*(cols-0.5),rowspace*rows/2)
		print "add 1x%02d/90 'ch1' R180 (%f %f);"%(cols,colspace*cols/2,rowspace/2.0,)
		print "add 1x%02d/90 'ch2' (%f %f);"%(cols,colspace*cols/2,rowspace*(rows-0.5))
		for r in range(rows):
			connect('r%d'%r,getpartname(r,1)     ,'1','rh1','%d'%(1+r))
			connect('r%d'%r,getpartname(r,cols-1),'1','rh2','%d'%(rows-r))
		for c in range(cols):
			connect('c%d'%c,getpartname(1,c)     ,'3','ch1','%d'%(cols-c))
			connect('c%d'%c,getpartname(rows-1,c),'3','ch2','%d'%(1+c))
	if addfineheaders:
		x1 = colspace;
		y1 = rowspace;
		x2 = colspace*cols-colspace;
		y2 = rowspace*rows-rowspace;
		xc = headerpitch*cols*0.5
		yc = headerpitch*rows*0.5
		xh = colspace*cols*0.5+0.25*colspace
		yh = rowspace*rows*0.5+0.25*rowspace
		ys = [i*headerpitch-yc+yh for i in range(rows)]
		xs = [i*headerpitch-xc+xh for i in range(cols)]
		for r in range(rows):
			print "add 1x01 'wh%d' (%f %f);"%(r,x1,ys[r])
			print "add 1x01 'eh%d' (%f %f);"%(r,x2,ys[r])
			connect('r%d'%r,'wh%d'%r,1,getpartname(r,1),'1')
			connect('r%d'%r,'eh%d'%r,1,getpartname(r,1),'1')
		for c in range(cols):
			print "add 1x01 'sh%d' (%f %f);"%(c,xs[c],y1)
			print "add 1x01 'nh%d' (%f %f);"%(c,xs[c],y2)
			connect('c%d'%c,'sh%d'%c,1,getpartname(1,c),'3')
			connect('c%d'%c,'nh%d'%c,1,getpartname(1,c),'3')
		print 'HOLE 0.12598425 (%f %f);'%(x1,y1)
		print 'HOLE 0.12598425 (%f %f);'%(x1,y2)
		print 'HOLE 0.12598425 (%f %f);'%(x2,y1)
		print 'HOLE 0.12598425 (%f %f);'%(x2,y2)
		
else:


	for r in range(rows):
		for c in range(cols):
			print "ADD '%s' R90 D%s (%f %f);"%(diode,getpartname(r,c),diodeoffset+c*colspace+0.5*colspace,r*rowspace+0.5*rowspace)
			if holes:
				print 'HOLE 0.12598425 (%f %f);'%(c*colspace+0.5*colspace,r*rowspace+0.5*rowspace)
			connect('s%d'%c,getpartname(r,c),'2','D'+getpartname(r,c),'A')
	for r in range(rows):
		use = set(range(cols))
		use.remove(r)
		use = list(use)
		for i in range(cols-2):
			connect('s%d'%r,'D'+getpartname(r,use[i]),cathodepinname,'D'+getpartname(r,use[i+1]),cathodepinname)
	for r in range(rows-1):
		connect('d','D'+getpartname(r,r),cathodepinname,'D'+getpartname(r+1,r+1),cathodepinname)
	for c in range(cols):
		for r in range(rows-1):
			connect('s%d'%c,'D'+getpartname(r,c),'A','D'+getpartname(r+1,c),'A')

	if addheaders:
		print "add 1x%02d/90 'h1' R180 (%f %f);"%(cols+1,colspace*cols/2,rowspace/2.0,)
		print "add 1x%02d/90 'h2' (%f %f);"%(cols+1,colspace*cols/2,rowspace*(rows-0.5))
		for c in range(cols):
			connect('s%d'%c,getpartname(1,c)     ,'3','h1','%d'%(1+cols-c))
			connect('s%d'%c,getpartname(rows-1,c),'3','h2','%d'%(1+c))
		connect('d',getpartname(0,0),'1','h2','%d'%(2+c))
		connect('d',getpartname(0,0),'1','h1','1')
	if addfineheaders:
		x1 = colspace;
		y1 = rowspace;
		x2 = colspace*cols-colspace;
		y2 = rowspace*rows-rowspace;
		xc = headerpitch*(cols)*0.5
		yc = headerpitch*rows*0.5
		xh = colspace*cols*0.5
		if cols>rows:
			xh = xh + 0.5*headerpitch
		xs = [i*headerpitch-xc+xh for i in range(rows+1)]
		for c in range(rows):
			print "add 1x01 'h%d' (%f %f);"%(c,xs[c],y1)
			connect('s%d'%c,'h%d'%c,1,getpartname(1,c),'3')
		print "add 1x01 'h%d' (%f %f);"%(rows,xs[-1],y1)
		connect('d','h%d'%rows,1,getpartname(1,1),'1')
		if cols>rows:
			connect('d','h%d'%rows,1,getpartname(1,rows),'3')
		print 'HOLE 0.12598425 (%f %f);'%(x1,y1)
		print 'HOLE 0.12598425 (%f %f);'%(x1,y2)
		print 'HOLE 0.12598425 (%f %f);'%(x2,y1)
		print 'HOLE 0.12598425 (%f %f);'%(x2,y2)
		if addchip:
			print "add %s 'chip' (%f %f);"%(chipname,xc,yc)
			for c in range(rows):
				connect('s%d'%c,'h%d'%c,1,'chip','%d'%chippins[c])
			connect('d','h%d'%rows,1,'chip','%d'%chippins[-1])
			

width = cols * colspace
height = rows * rowspace
print "layer 'Dimension';"
print "wire 0 (0 0) (0 %(height)f);"%{'height':height,'width':width}
print "wire 0 (0 0) (%(width)f 0);"%{'height':height,'width':width}
print "wire 0 (0 %(height)f) (%(width)f %(height)f);"%{'height':height,'width':width}
print "wire 0 (%(width)f 0) (%(width)f %(height)f);"%{'height':height,'width':width}
print 'ratsnest;'
#print 'auto;'


print ''





