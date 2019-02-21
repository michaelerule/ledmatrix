#!/usr/bin/env python
from pylab import *

fn = 'bach_toccata.mid' #good

data = ''
with open(fn,'rb') as f:
	b = f.read(1)
	while b:
		data = data + b
		b = f.read(1)

data = data.split('MTrk')[1:]
data = [map(ord,d) for d in data]

def eat(s):
	return s[0],s[1:]

def eatVarint(s):
	k,s = eat(s)
	x = 0
	x = k&0x7f
	while k&0x80:
		x = x*128+k&0x7f
		k,s=eat(s)
	return x,s
		
def eatEvent(s):
	k,s = eat(s)
	e = k >> 4
	if e is 0x9: #note on
		ch = k&0xf
		note,s = eat(s)
		volume,s = eat(s)
		return ('on',ch,note,volume),s
	if e is 0x8: #note off
		ch = k&0xf
		note,s = eat(s)
		volume,s = eat(s)
		return ('off',ch,note,volume),s
	if e is 0xff:
		type,s=eat(s)
		length,s=eatVarint(*s)
		for i in range(length):
			_,s=eat(s)
		return None,s
	if e is 0xf0 or e is 0xf7:
		length,s=eatVarint(*s)
		for i in range(length):
			_,s=eat(s)
		return None,s
	return None,s
		
song = []
a = []
while data:
	a = a + data[0]
	data = data[1:]
ii=0
jj=0
current = None
while len(a):
	i,a = eatVarint(a)
	ii = ii + i
	x,a = eatEvent(a)
	if x:
		status,ch,note,volume = x
		'''
		if (status=='off' or volume==0) and current:
			j,x=current
			d =ii-jj
			jj=ii
			if d>10:
				song.append((d,x))
			current=None
		elif status=='on' and volume>0:
			current = ii,note
		'''
		if status=='on' and volume>0:
			if current:
				j,x=current
				d =ii-j
				if d>10:
					song.append((d,x))
			current = ii,note

if current:
	j,x=current
	d =ii-j
	if d>10:
		song.append((d,x))

s = [a[1] for a in song if a[0]>5]
z = [a[0] for a in song if a[0]>5]
z = int32(z)/16
m = min(s)
song = [x-m for x in s]











	
