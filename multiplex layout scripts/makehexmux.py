#!/usr/bin/env python
'''
    5 5 5 5 5
   5 4 4 4 4 5
  5 4 3 3 3 4 5
 5 4 3 2 2 3 4 5
5 4 3 2 1 2 3 4 5
 5 4 3 2 2 3 4 5
  5 4 3 3 3 4 5
   5 4 4 4 4 5
    5 5 5 5 5
'''

import sys,os
from math import *

order    = 5
spacing  = 0.2
#spacing  = 5
partname = 'LED3MM'
cathode  = 'K'
anode    = 'A'
charlie  = 1
headers  = 0
finehead = 0
headspace= 0.1
elements = 1+6*sum(range(order))

enumeration = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'

pins     = 2
while (pins*(pins-1)<elements) :
	pins = pins+1

def add(part,name,x,y,r=0):
	print "ADD '%s' %s R%d (%f %f);"%(part,name,r,x,y)

def connect(name,part1,pad1,part2,pad2):
	print "SIGNAL %s %s %s %s %s;"%(name,part1,pad1,part2,pad2)

import base64
import struct
 
def encode(n):
  data = struct.pack('<Q', n).rstrip('\x00')
  if len(data)==0:
    data = '\x00'
  s = base64.urlsafe_b64encode(data).rstrip('=')
  return s
 
def decode(s):
  data = base64.urlsafe_b64decode(s + '==')
  n = struct.unpack('<Q', data + '\x00'* (8-len(data)) )
  return n[0]

def getpartname(n):
	return str(n)

columns = [i+order for i in range(order)]+[i+order for i in range(order-2,-1,-1)]

vspace = sqrt(3)*0.5*spacing
width  = spacing*(order*2-1)
height = vspace*(order*2-1)
center = width*0.5
grid   = int(ceil(sqrt(pins)))

1.9685

if (height>1.9685 or width>1.9685):
    print "TOO BIG"
    sys.err.write('AFSDGA')
    exit(0)

if (height>3.2 or width>4):
    print "TOO BIG"
    sys.err.write('AFSDGA')
    exit(0)

y = 0.5*vspace
n = 0

for row,cols in enumerate(columns):
	rowidth = spacing*(cols-1)
	x = center-0.5*rowidth
	for i in range(cols):
		add(partname,getpartname(n),x,y)
		x = x+spacing
		n = n+1
	y = y+vspace

for i in range(pins):
	row = i/grid
	col = i%grid
	print "add 1x01 'h%d' (%f %f);"%(i,(0.5+col)*headspace,(0.5+row)*headspace)

for n in range(elements):
	row = n/(pins-1)
	col = n%(pins-1)
	name = getpartname(n)
	connect('s%d'%col,name,cathode,'h%d'%col,'1')
	if row==col:
		row = pins-1
	connect('s%d'%row,name,anode,'h%d'%row,'1')

inset = height/sqrt(3)*0.5

outline = [\
	(inset,0),
	(width-inset,0),
	(width,height*0.5),
	(width-inset,height),
	(inset,height),
	(0,height*0.5)]

print "layer 'Dimension';"
print "SET WIRE_BEND 2;"
for i in range(len(outline)):
	x1,y1 = outline[i];
	x2,y2 = outline[(i+1)%len(outline)];
	print "wire 0 (%f %f) (%f %f);"%(x1,y1,x2,y2)

print 'ratsnest;'


