#!/usr/bin/env python
from math import *

song = list(reversed(range(14)))

#use = [0,2,4,5,7,9,11] #major
#use = [0,2,3,5,7,8,10] #minor
use = [0,3,5,6,7,10] #blues
#use = range(12)

topf    = 1./24
bottomf = 1./360

k = 0
notes = []
while bottomf*2**(k/12.)<=topf:
	if k%12 in use:
		notes.append( int( 1./(bottomf*2**(k/12.)) ))
	k = k+1

offset = len(notes)-max(song)-1
notes = [notes[i+offset] for i in song]

pitches = list(set(notes))

notebits = ceil(log(len(pitches))/log(2))
lengthbits = 8-notebits
lengthmax = 2**lengthbits-1
notes = ['0x%04X'%(256*(n/2+5/4)+pitches.index(i)) for n,i in enumerate(notes)]

unique = list(set(notes))

notes = [unique.index(i) for i in notes]

print '#define  BLUES_NNOTES  %d'%len(notes)
print '#define  BLUES_NUNIQUE %d'%len(unique)
print '#define  BLUES_NPITCH  %d'%len(pitches)
print 'uint16_t BLUES_pitches[NPITCH]  = {'+', '.join(map(str,pitches))+'};'
print 'uint16_t BLUES_unique [NUNIQUE] = {'+', '.join(map(str,unique))+'};'
print 'const uint8_t  BLUES_notes  [NNOTES]  PROGMEM = {'+', '.join(map(str,notes))+'};'



