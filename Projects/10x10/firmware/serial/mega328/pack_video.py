#!/usr/bin/env python
import serial,os,sys,time,tty,termios
from pylab import *
from PIL import Image

N = 10
framebytes = (2*N)

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
    data = int32(exp(float32(list(Image.open('../video/old/'+f).getdata())).reshape((10,10))/127.))-1
    #data = int32(list(Image.open('../video/'+f).getdata())).reshape((10,10))/32
    return makeFrame(data)

room = (32*1024-1208)*8

files = sorted([s for s in os.listdir('../video/old/') if '.png' in s])
frames = map(getFrame,files[:min(room/300,len(files))])
NFRAME = len(frames)

bitindex = 0
stream = []
byte = 0
def write_bit(b):
    global bitindex,stream,byte
    i = bitindex%8
    byte=byte|(b<<i)
    bitindex=bitindex+1
    if bitindex%8==0:
        stream.append(byte)
        byte = 0
for f in frames:
    for i in f[:10]:
        for b in range(9*3):
            write_bit((i/2**b)%2)
    i = f[-1]
    for b in range(10*3):
        write_bit((i/2**b)%2)
if bitindex%8!=0:
    stream.append(byte)
print '#define VIDEO_LENGTH_FRAMES %d'%NFRAME
NBITS = NFRAME*300
print '#define VIDEO(i) (pgm_read_byte_near(&video[(i)]))'
print '#define VIDEO_LENGTH_BITS %d\n'%NBITS
s = '{'
i = 0
while i<len(stream):
    if i%38==0:
        s = s+'\n'
    s = s+'0x%02X'%stream[i]+','
    i=i+1
s=s+'};'
print 'uint8_t video[] PROGMEM = \n%s'%s





