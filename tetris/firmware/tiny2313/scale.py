#!/usr/bin/env python
from math import *

execfile('midi.py')

use = range(12)

topf    = 1./42
bottomf = 1./360

k = 0
notes = []
while bottomf*2**(k/12.)<=topf:
	if k%12 in use:
		notes.append( int( 1./(bottomf*2**(k/12.)) ))
	k = k+1

#song = [i%12 for i in song]
offset = len(notes)-max(song)-1
notes = [notes[i+offset] for i in song]

pitches = sorted(list(set(notes)))
notes = [pitches.index(i) for i in notes]

print '#define NPITCH  %d'%len(pitches)
print '#define NNOTES  %d'%len(notes)
print 'uint8_t pitches[NPITCH] PROGMEM = {'+','.join(map(str,pitches))+'};'
print 'uint8_t notes  [NNOTES] PROGMEM = {'+','.join(map(str,notes))  +'};'


