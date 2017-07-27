#!/usr/bin/env python
from math import *
"""
'''

E F# G G F# E D# D# E F# B B C# D# E E D  C B B A G F# F# G A B A G F# E E 
0 2  3 3 2 0  11 11 0 2  7 7 9  11 0 0 10 8 7 7 5 3 2 2   3 5 7 5 3 2 0 0
-2 -9 -7 -7 3 -2 


'''
song = map(eval,'''
0 2 3 3 2 0 -1 -1 0 2 -5 -5 -3 -1 0 0 -2 -4 -5 -5 -7 -9 -10 -10 -9 -7 -5 -7 -9 -10 -12 -12 
0 2 3 3 2 0 -1 -1 0 2 -5 -5 -3 -1 0 0 -2 -4 -5 -5 -7 -9 -10 -10 -10 -9 -9
-5 -9 -2 -7 -4 -5 -5 3 -2 0 0 -5 -2 -4 -5 -7 -8 -8 -7 -5 -4 -4 -5  -7 -5 -7 -9 -10 -10
-5 -9 -2 -7 -4 -5 -5 3 -2 0 0 -5 -2 -4 -5 -7 -8 -8 -7 -5 -4 -4 -5  -7 -5 -7 -9 -10 -10
'''.split())
song = map(eval,'''
0 2 3 2 0 -1 0 2 -5 -3 -1 0 -2 -4 -5 -7 -9 -10 -9 -7 -5 -7 -9 -10 -12 
0 2 3 2 0 -1 0 2 -5 -3 -1 0 -2 -4 -5 -7 -9 -10 -9
-5 -9 -2 -7 -4 -5 3 -2 0 -5 -2 -4 -5 -7 -8 -7 -5 -4 -5  -7 -5 -7 -9 -10
-5 -9 -2 -7 -4 -5 3 -2 0 -5 -2 -4 -5 -7 -8 -7 -5 -4 -5  -7 -5 -7 -9 -10
'''.split())
song = map(eval,'''
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
'''.split())

song = []

for i in range(2,2*6):
	song = song + range(6,6+i) + list(reversed(range(6,6+i-1)))

song = map(eval,'''
0 2 3 2 0 -1 0 2 -5 -3 -1 0 -2 -4 -5 -7 -9 -10 -9 -7 -5 -7 -9 -10 -12 
0 2 3 2 0 -1 0 2 -5 -3 -1 0 -2 -4 -5 -7 -9 -10 -9
-5 -9 -2 -7 -4 -5 3 -2 0 -5 -2 -4 -5 -7 -8 -7 -5 -4 -5  -7 -5 -7 -9 -10
-5 -9 -2 -7 -4 -5 3 -2 0 -5 -2 -4 -5 -7 -8 -7 -5 -4 -5  -7 -5 -7 -9 -10
'''.split())
#song = map(int,'0 2 3 3 2 0 11 11 0 2 7 7 9 11 0 0 10 8 7 7 5 3 2 2 3 5 7 5 3 2 0 0'.split())
"""
song = map(eval,'''24 26 14 29 24 24 29 31 33 35 24 24 31 29 11 26 29 28 17 24 16 26 12 24 7 17 16 14 24 26 14 29 16 12 29 31 33 35 17 16 14 12 31 29 11 26 29 28 16 24 19 26 7 23 36 38 40 36 31 33 35 31 9 12 35 9 31 28 30 14 28 30 31 33 35 19 18 19 11 14 19 23 21 23 16 14 12 19 24 23 24 7 21 23 24 11 28 9 12 11 12 31 4 7 12 7 0'''.split())

execfile('midi.py')

#use = [0,2,4,5,7,9,11] #major
#use = [0,2,3,5,7,8,10] #minor
#use = [0,3,5,6,7,10] #blues
use = range(12)

topf    = 1./18
bottomf = 1./360

k = 0
notes = []
while bottomf*2**(k/12.)<=topf:
	if k%12 in use:
		notes.append( int( 1./(bottomf*2**(k/12.)) ))
	k = k+1

print '// %d available notes'%len(notes)
print '// song spans %d notes'%(max(song)-min(song))

offset = len(notes)-max(song)-1
notes = [notes[i+offset] for i in song]

pitches = list(set(notes))

notebits = ceil(log(len(pitches))/log(2))
lengthbits = 8-notebits
lengthmax = 2**lengthbits-1
print '// need %d bits for notes'%notebits
print '// lengthmax is %d'%lengthmax
notes = ['0x%04X'%(256*d+pitches.index(i)) for d,i in zip(z,notes)]
print '// combo set set is %s'%len(list(set(notes)))

unique = list(set(notes))

notes = [unique.index(i) for i in notes]

print '#define NNOTES  %d'%len(notes)
print '#define NUNIQUE %d'%len(unique)
print '#define NPITCH  %d'%len(pitches)
print 'uint16_t  pitches[NPITCH] = {'+', '.join(map(str,pitches))+'};'
print 'uint16_t unique[NUNIQUE] = {'+', '.join(map(str,unique))+'};'
print 'const uint8_t  notes[NNOTES]   PROGMEM = {'+', '.join(map(str,notes))+'};'



