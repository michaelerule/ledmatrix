#!/usr/bin/env python
import serial,os,sys,time,tty,termios
from pylab import *
from PIL import Image

N = 10
framebytes = (2*N)
FRAMEBITS = 200
COLORDEPTH = 2
room = (32*1024-1320)*8-400
WORDLEN = 8
video_dir = '../video/old/'
#video_dir = '../video/'

def binary2int(b):
    result = 0
    for bit in b:
        result = result*2
        if bit:
            result = result+1
    return result

def makeFrame(data):
    words = []
    extra_ones = []
    extra_twos = []
    extra_four = []
    for j,row in enumerate(data) : 
        row = int32(row)
        ones = list(row % 2)
        twos = list(row % 4 / 2)
        four = list(row % 8 / 4)
        i = 9-j
        extra_ones.append(ones[i])
        extra_twos.append(twos[i])
        extra_four.append(four[i])
        ones = ones[:i]+ones[i+1:]
        twos = twos[:i]+twos[i+1:]
        four = four[:i]+four[i+1:]
        bits = four+twos+ones
        words.append(binary2int(bits))
    extra_ones.reverse()
    extra_twos.reverse()
    extra_four.reverse()
    bits = extra_four+extra_twos+extra_ones
    words.append(binary2int(bits))
    return words

def getFrame(f):
    data = float32(list(Image.open(video_dir+f).getdata()))
    data = data.reshape((10,10))
    data = data * 4 / 256.
    data = int32(data)
    return makeFrame(data)

files = sorted([s for s in os.listdir(video_dir) if '.png' in s])
files = files[::7]
frames = map(getFrame,files[:min(room/FRAMEBITS,len(files))])

frames = [makeFrame(int32(zeros((10,10)))) for i in range(2)] + frames


NFRAME = len(frames)

bitindex = 0
stream = []
byte = 0
def write_bit(b):
    global bitindex,stream,byte
    i = bitindex%WORDLEN
    byte=byte|(b<<i)
    bitindex=bitindex+1
    if bitindex%WORDLEN==0:
        stream.append(byte)
        byte = 0
        
for f in frames:
    for i in f[:10]:
        for b in range(9*COLORDEPTH):
            write_bit((i/2**b)%2)
    i = f[-1]
    for b in range(10*COLORDEPTH):
        write_bit((i/2**b)%2)


if bitindex%WORDLEN!=0:
    stream.append(byte)
    
NBITS = NFRAME*FRAMEBITS
print '#define v_type uint%d_t'%WORDLEN
print '#define DATALEN %d'%(WORDLEN)
print '#define DATALENMASK %d'%(WORDLEN-1)
print '#define VIDEO_LENGTH_FRAMES %d'%NFRAME
print '#define VIDEO_LENGTH_WORDS %d'%int32(ceil(NBITS/float(WORDLEN)))
print '#define VIDEO_LENGTH_BITS %d\n'%NBITS
print '#define VIDEO(i) (pgm_read_byte(&video[(i)]))'
s = '{'
i = 0
def b2s(i):
    return ('0x%%0%dx'%(WORDLEN/4))%i
while i<len(stream):
    if i%25==0:
        s = s+'\n'
    s = s+b2s(stream[i])+','
    i=i+1
s=s+'};'
print 'uint%d_t video[] PROGMEM = \n%s'%(WORDLEN,s)





