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
    data = int32(exp(float32(list(Image.open('./video/'+f).getdata())).reshape((10,10))/127.))-1
    return makeFrame(data)
    
#print sorted([s for s in os.listdir('./video') if '.png' in s])

frames = map(getFrame,sorted([s for s in os.listdir('./video') if '.png' in s]))
NFRAME = len(frames)

bitindex = 0
stream = []
byte = 0

def write_bit(b):
    global bitindex,stream,byte
    i = bitindex%8
    byte=byte|(b<<i)
    bitindex=bitindex+1
    if bitindex%8==7:
        stream.append(byte)
        byte = 0

for f in frames:
    for i in f[:10]:
        for b in range(9*3):
            write_bit((i/2**b)%2)
    i = f[-1]
    for b in range(10*3):
        write_bit((i/2**b)%2)

if bitindex%8!=7:
    stream.append(byte)

print '#define VIDEO_LENGTH_BITS %d\n'%(NFRAME*300)
print 'uint8_t video[] PROGMEM = \n{'+','.join(['0x%02x'%b for b in stream])+'};'





