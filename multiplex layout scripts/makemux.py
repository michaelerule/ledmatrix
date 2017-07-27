#!/usr/bin/env python

'''
outputs a series of commands to build a multiplexd array in eagle
'''
cols = 16
rows = 16

#partname = 'CHIP-LED0603'
#partname = 'CHIP-LED0805'
partname = 'LED3MM'
#partname = 'LED5MM'
#partname = 'LED10MM'
#rowspace = 0.11
rowspace = 0.2
#rowspace = 0.3
#rowspace = 0.248998241 #tight 5mm spacing
#rowspace = 0.25 #tight 5mm spacing
#rowspace = 3.2/cols
#rowspace = 0.26
#rowspace = 0.45
#rowspace = 0.5

cathode_name = 'K'
#cathode_name = 'C'

#TSW-1%0d-02-S-S-RA

def add(part,name,x,y,r=0):
	print "ADD '%s' %s R%d (%f %f);"%(part,name,r,x,y)

def connect(name,part1,pad1,part2,pad2):
	print "SIGNAL %s %s %s %s %s;"%(name,part1,pad1,part2,pad2)

enumeration = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'

'''
boardsize = float(3.2)*3/4
rowspace = boardsize/rows
colspace = boardsize/cols
'''

colspace = rowspace

rot45 =1 
interflip=0
flipflip=0.5 if rot45 else 0
charlieplex = 1
addheaders = 0
addfineheaders = 1
headerpitch = 0.1
addchip = 0
#chipname = 'SOCKET-14' #'SO14'
#chippins = [2,3,4,5,6,7,8,9,13,12,10]

chipname = 'SO14@maxim'
chippins = [2,3,4,5,6,7,8,9,13,12,10]

boardheight=rowspace*rows
boardwidth=colspace*cols
'''
if (boardheight>3.2 or boardwidth>4):
	print "TOO BIG"
	exit(0)
'''
def getpartname(r,c):
	return '%s%s'%(enumeration[r],enumeration[c])

if interflip:
	for r in range(rows):
		for c in range(cols):
			if ((r&1)==(c&1)):
				add(partname,getpartname(r,c),c*colspace+0.5*colspace,r*rowspace+0.5*rowspace,90)
			else:
				add(partname,getpartname(r,c),c*colspace+0.5*colspace,r*rowspace+0.5*rowspace)
else:
	for r in range(rows):
		for c in range(cols):
			add(partname,getpartname(r,c),c*colspace+0.5*colspace,r*rowspace+0.5*rowspace,90*flipflip)

if not charlieplex:
	for r in range(rows):
		for c in range(cols-1):
			connect('r%d'%r,getpartname(r,c),'A',getpartname(r,c+1),'A')
	for c in range(cols):
		for r in range(rows-1):
			connect('c%d'%c,getpartname(r,c),cathode_name,getpartname(r+1,c),cathode_name)
	if addheaders:
		print "add 1x%02d/90 'rh1' R90 (%f %f);"%(rows,colspace/2.0,rowspace*rows/2)
		print "add 1x%02d/90 'rh2' R270 (%f %f);"%(rows,colspace*(cols-0.5),rowspace*rows/2)
		print "add 1x%02d/90 'ch1' R180 (%f %f);"%(cols,colspace*cols/2,rowspace/2.0,)
		print "add 1x%02d/90 'ch2' (%f %f);"%(cols,colspace*cols/2,rowspace*(rows-0.5))
		for r in range(rows):
			connect('r%d'%r,getpartname(r,1)     ,'A','rh1','%d'%(1+r))
			connect('r%d'%r,getpartname(r,cols-1),'A','rh2','%d'%(rows-r))
		for c in range(cols):
			connect('c%d'%c,getpartname(1,c)     ,cathode_name,'ch1','%d'%(cols-c))
			connect('c%d'%c,getpartname(rows-1,c),cathode_name,'ch2','%d'%(1+c))
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
			connect('r%d'%r,'wh%d'%r,1,getpartname(r,1),'A')
			connect('r%d'%r,'eh%d'%r,1,getpartname(r,1),'A')
		for c in range(cols):
			print "add 1x01 'sh%d' (%f %f);"%(c,xs[c],y1)
			print "add 1x01 'nh%d' (%f %f);"%(c,xs[c],y2)
			connect('c%d'%c,'sh%d'%c,1,getpartname(1,c),cathode_name)
			connect('c%d'%c,'nh%d'%c,1,getpartname(1,c),cathode_name)
		print 'HOLE 0.12598425 (%f %f);'%(x1,y1)
		print 'HOLE 0.12598425 (%f %f);'%(x1,y2)
		print 'HOLE 0.12598425 (%f %f);'%(x2,y1)
		print 'HOLE 0.12598425 (%f %f);'%(x2,y2)
		
else:
	for r in range(rows):
		use = set(range(cols))
		use.remove(r)
		use = list(use)
		for i in range(cols-2):
			connect('s%d'%r,getpartname(r,use[i]),'A',getpartname(r,use[i+1]),'A')
	for c in range(cols):
		for r in range(rows-1):
			connect('s%d'%c,getpartname(r,c),cathode_name,getpartname(r+1,c),cathode_name)
	for r in range(rows-1):
		connect('d',getpartname(r,r),'A',getpartname(r+1,r+1),'A')
	if addheaders:
		print "add 1x%02d/90 'h1' R180 (%f %f);"%(cols+1,colspace*cols/2,rowspace/2.0,)
		print "add 1x%02d/90 'h2' (%f %f);"%(cols+1,colspace*cols/2,rowspace*(rows-0.5))
		for c in range(cols):
			connect('s%d'%c,getpartname(1,c)     ,cathode_name,'h1','%d'%(1+cols-c))
			connect('s%d'%c,getpartname(rows-1,c),cathode_name,'h2','%d'%(1+c))
		connect('d',getpartname(0,0),'A','h2','%d'%(2+c))
		connect('d',getpartname(0,0),'A','h1','1')
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
			connect('s%d'%c,'h%d'%c,1,getpartname(1,c),cathode_name)
		print "add 1x01 'h%d' (%f %f);"%(rows,xs[-1],y1)
		connect('d','h%d'%rows,1,getpartname(1,1),'A')
		if cols>rows:
			connect('d','h%d'%rows,1,getpartname(1,rows),cathode_name)
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





