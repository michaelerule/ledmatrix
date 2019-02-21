#!/usr/bin/env ipython
# -*- coding: utf-8 -*-
from string import *
from pylab import *
from scipy.io import *
import os,sys
from collections import *
execfile('seven.py')


notWords = '''b c d e f g h j k l m n o p q r s t u v w x y z - -- -p -u donut que les ecus cur qui quae quod '''.split()

minp = 0.0001

frequency = defaultdict(int)
markov = defaultdict(lambda: defaultdict(int))
for f in os.listdir('./corpus'):
	f = open('./corpus/'+f,'r')
	prev = None
	special = None
	for l in f.readlines():
		for w in strip(l).split():
			w=clean(w)
			if not (w=='' or w in notWords):
				if prev!=None:
					markov[prev][w] += 1
				if prev!=None and prev in ['the','a'] and special!=None:
					markov[special][w] += 1
				special = prev
				prev = w
				frequency[w]+=1

"""
Now, the challenge -- remove all words not printable on a 7-segment display
while keeping word frequencies of remaining words in the markov model 
unchanged.

My approach: get the raw frequencies from the corpus. delete the invalid
words. Then what? compute the eigenvalues of the new system and correct.
How? I do not know.
"""

# freeze it as a dictionary
markov = dict(markov)
for k in markov.keys():
	markov[k] = dict(markov[k])
# remove unprintable words
for k in markov.keys():
	if not can7(k):
		del markov[k]
# remove unprintable children
remain = set(markov.keys())
finalized = set(remain)
for k,v in markov.iteritems():
	for l in v.keys():
		if not l in remain:
			del markov[k][l]
		else:
			finalized.add(l)
finalized = sorted(list(set(finalized)))
print finalized

#convert to array
n = len(finalized)
graph = float64(zeros((n,n)))
for i,w in enumerate(finalized):
	for k,v in markov[w].iteritems():
		j = [j for j,x in enumerate(finalized) if x==k][0]
		graph[i,j]=v
print graph

'''
seed = 'a'
seed = [j for j,x in enumerate(finalized) if x==seed][0]
vector = float64(zeros((n,1)))
vector[seed] = 1
for i in range(15):
	graph = dot(graph,graph)
	graph /= np.max(graph)
	for j in range(150):
		old   = array(vector)
		vector = dot(graph,vector)
		vector /= np.max(vector)
		diff = sum(abs(seed-old)**2)
		if diff<0.001:
			break
vector /= sum(vector)
freqs = [(finalized[i],vector[i,0]) for i in reversed(argsort(vector[:,0]))]
keep = set([f for f,p in freqs if p>0.0])
freqs = dict(freqs)
# remove unreachable words
for k in markov.keys():
	if not k in keep:
		del markov[k]
for k in frequency.keys():
	if not k in keep:
		del frequency[k]
'''
w,n = zip(*frequency.iteritems())
frequency = float64(array(n))
frequency /= sum(array(n))
frequency = dict(zip(w,frequency))
'''
adjust = defaultdict(float)
for k in keep:
	adjust[k] = frequency[k]/freqs[k]
print adjust
'''
for k,v in markov.iteritems():
	for l in v.keys():
		markov[k][l] *= sqrt(frequency[l])

def generate():
	prev = markov.keys()
	prev = prev[int(random()*len(prev))]
	while 1:
		if random()<minp:
			next = markov.keys()
			next = next[int(random()*len(next))]
		else:
			n = random()*sum(markov[prev].values())
			j = 0.0
			for k,v in markov[prev].iteritems():
				j += v
				if n<j:
					next = k
					break
		yield next
		if not next in markov or len(markov[next])<1:
			prev = markov.keys()
			prev = prev[int(random()*len(prev))]
		else:	
			prev = next

g = generate()
for i in range(1000):
	print g.next(),

"""
To actually send to serial, we need some special Glyphs for
ii II ll li il Ii
These need three glyps: ll, li, il, ii The binary codes are
	0b11001110
	0b11001010
	0b11000110
	0b11000010
We also need i_ _i l_ and _l, these are:
	0b11000000
	0b10000010
	0b11001000
	0b10000110
In decimal these are:
	0b11001110 206 ll
	0b11001010 202 li
	0b11000110 198 il
	0b11000010 194 ii
	0b11000000 192 i_
	0b10000010 130 _i
	0b11001000 200 l_
	0b10000110 134 _l
We need to modify the to7 function to understand these
"""


halfMap = {'i_':128+1,
'_i':chr(128+32),
'l_':chr(128+1+8),
'_l':chr(128+32+16),
'il':chr(128+1+32+16),
'li':chr(128+1+8+32),
'ii':chr(128+1+32),
'll':chr(128+1+8+32+16)}
def asBytes(s):
	return [ord(c) for c in s]

def handleHalfWidth(s,c,x,y,z):
	d = ''.join([c,c])
	if not c in s:
		return s
	ss = s.replace(d,z)
	if not c in ss:
		return ss
	if s[0]==c and s[-1]==i:
		ss = s[1:-1].replace(d,z)
		if not c in s[1:-1].replace(d,z):
			return x+ss+y
	if s[0]==c:
		ss = s[1:].replace(d,z)
		if not c in ss:
			return x+ss
	if s[-1]==c:
		ss = s[:-1].replace(d,z)
		if not c in ss:
			return ss+y
	return None

def handleILHalfWidth(s):
	#if s[:2] in ['ii','il']:
	#	return None
	z = s.replace('i','l')
	capitalL = 0
	w = handleHalfWidth(z,'l','$','^','#')
	if s[0]=='l':
		v = handleHalfWidth(z[1:],'l','$','^','#')
		if v!=None:
			capitalL = 1
			w = v
	if w==None:
		return None
	i = 0
	cleaned = ''
	for c in w:
		if c=='$':
			assert not i
			assert s[0]=='i' or s[0]=='l'
			c = halfMap['_'+s[0]]
		elif c=='^':
			assert i==len(s)-1
			assert s[-1]=='i' or s[-1]=='l'
			c = halfMap[s[-1]+'_']
		elif c=='#':
			c = halfMap[s[i:i+2]]
			i += 1
		cleaned += c
		i += 1
	return 'L'+cleaned if capitalL else cleaned

def to7(s):
	s = clean(s)
	if not can7(s):
		return None
	# prefer title-case or lower case
	if all([c in lower for c in s[1:]]):
		z = handleILHalfWidth(s)
		if z==None:
			return handleHalfWidth(s.upper(),'I',halfMap['_l'],halfMap['l_'],halfMap['ll'])
		if z[0] in upper:
			return z[:1].upper()+z[1:]
		# special case: z[0] was the letter i
		if z[0]==halfMap['_i']:
			return halfMap['_l']+z[1:]
		return z
	# uppercase fallback
	return handleHalfWidth(s.upper(),'I',halfMap['_l'],halfMap['l_'],halfMap['ll'])
		
import serial,os,sys,time,tty,termios
from pylab import *
port     = '/dev/ttyUSB0'
MAXBYTES = 1000
baudrate = 9600

def startSerial(baudrate):
	os.system('stty -F %s %d cs8 cread'%(port,baudrate))
	ser = serial.Serial(port,baudrate,timeout=0)
	time.sleep(.1)
	if not ser.isOpen():
	    print 'CONNECTION FAILED'
	    sys.exit(0)
	print 'SUCCESS'
	return ser
	
def closeSerial(ser):
	ser.close()
	print 'CLOSED'
	 
def sendWord(w):
	v = to7(w)
	if v==None:
		print 'CANT CONVERT',w
		return
	v = v.replace('T','t')
	for c in ' '+v:
		ser.write(c)
		for j in range(120):
			time.sleep(0.001)
			
ser = startSerial(baudrate)

#for i in range(100):
while 1:
	sendWord(g.next())
	
closeSerial(ser)







