#!/usr/bin/env python
'''
Extreme bit packing version
'''

import serial,os,sys,time,tty,termios
from pylab import *
from PIL import Image

N = 10
framebytes = (2*N)
FRAMEBITS = 200
COLORDEPTH = 2
room = (32*1024-1320)*8-400
WORDLEN = 8
scale=256
video_dir = '/home/mrule/Desktop/frames/CGID3_85-110HZ_POWER_2HZ_LOWPASS_TRIPLE_BANDPASS/'

def b2i(b):
    i = 0
    for t in b:
        i = i*2
        if t:
            i = i+1
    return i

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
        words.append(b2i(bits))
    extra_ones.reverse()
    extra_twos.reverse()
    extra_four.reverse()
    bits = extra_four+extra_twos+extra_ones
    words.append(b2i(bits))
    return words

def getFrame(f):
    data = int32(list(Image.open(video_dir+f).getdata()))
    data = data.reshape((10,10))
    data = data * 4 / scale
    data = int32(data)
    return data

def approximate(model,frame):
    index = 0
    ixs = []
    for i in range(10):
        for j in range(10):
            index = index+1
            residual = frame[i,j]-model[i,j]
            if residual:
                if residual>0:
                    ixs.append(index*2+1)
                    model[i,j] = model[i,j]+1
                if residual<0:
                    ixs.append(index*2)
                    model[i,j] = model[i,j]-1
                index = 0
    return model, ixs

def cl2(i):
    return int(ceil(log(i+1)/log(2)))

def i2b(i,n):
    if n<cl2(i):
        print "ERROR NOT ENOUGH BITS !"
        return None
    seq = []
    for k in range(n):
        seq.append( (i/2**k)%2 )
    return list(reversed(seq))

def vbix(i):
    seq = []
    for j in range(20):
        if i<8:
            seq.append(1)
            seq.extend(i2b(i,3))
            return seq
        seq.append(0)
        i = i-8
    seq.extend(i2b(i,7))    
    return seq

def unpack(delta):
    index = -1
    new = int32(zeros((10,10)))
    for event in delta:
        dx    = 1 if (event % 2==1) else -1
        di    = int(event / 2)
        index = index + di
        i     = index / 10
        j     = index % 10
        new[i][j] = dx
        #print event,dx,di,index,i,j,new[i][j]
    return new   

def printArray(bb):
    s = '{'
    l = ''
    for i,b in enumerate(bb):
        l = l+'0x%02x'%b+','
        if len(l)>100:
            s = s + '\n' + l
            l = ''
    s = s + '\n' + l
    s=s+'};'
    return s

files = sorted([s for s in os.listdir(video_dir) if '.png' in s])
#frames = map(getFrame,files)
frames = map(getFrame,files[:840])

f = array(frames)
g = f[1:]-f[:-1]

filedata = ''

model = frames[0]
delta_frames = []
model_frames = [model]
rawindexes = []

for frame in frames[1:]:
    model,delta = approximate( array(model), frame )
    delta_frames.append(array(delta))
    model_frames.append(array(model))
    delta.append(0)
    data = []
    for i in delta:
        data.extend(vbix(i))
    filedata = filedata+''.join(map(str,data))

sys.stderr.write('\n'.join([str(unpack(f)) for f in delta_frames]))
sys.stderr.write('\n')
sys.stderr.write('\n'.join([str(f) for f in delta_frames]))
sys.stderr.write('\n')
sys.stderr.write('\n'.join([','.join([str(vbix(i)) for i in f]) for f in delta_frames]))
sys.stderr.write('\n')
sys.stderr.write('\n'.join([','.join([str(b2i(vbix(i))) for i in f]) for f in delta_frames]))
sys.stderr.write('\n')
sys.stderr.write(filedata)
sys.stderr.write('\n')

'''
# test decodability
bitstream = map(int,filedata)

def nextBit():
    global bitstream
    x = bitstream[0]
    bitstream = bitstream[1:]
    return x

def dvbix():
    global bitstream
    offset = 0
    while 1:
        if nextBit():
            x = 0
            for i in range(4):
                x = x*2
                if nextBit():
                    x = x+1
            return x + offset
        offset = offset + 16

def bframe():
    data = []
    i = dvbix()
    while i:
        data.append(i)
        i = dvbix()
    return data

def dframe(previous):
    new = int32(previous)
    index = -1
    for event in bframe():
        delta = (event % 2)*2-1
        di    = event / 2
        index = index + di
        i     = index / 10
        j     = index % 10
        new[i][j] = previous[i][j]+delta
    return new

print sum(abs(model_frames-f))
model = array(frames[0])
for i,frame in enumerate(model_frames[1:]):
    model = dframe(array(model))
    print sum(abs(frame-model)),
'''
'''
pack for output
first 200 bits are the 0th (key) frame
aftewards start 0-terminated lists of variable bit length update events
'''

alldata = map(int,filedata)
NBITS = len(alldata)
while len(alldata)%8:
    alldata.append(0)

kf = list(array([list(reversed(i2b(i,2))) for i in frames[0].flatten()]).flatten())
sys.stderr.write(str(kf))
sys.stderr.write('\n')
sys.stderr.write(str([list(reversed(kf[i*8:(i+1)*8])) for i in range(len(kf)/8)]))
sys.stderr.write('\n')
keyframe = [b2i(reversed(kf[i*8:(i+1)*8])) for i in range(len(kf)/8)]
sys.stderr.write(''.join(['%02x'%d for d in keyframe]))
sys.stderr.write('\n')
allbytes = [b2i(reversed(alldata[i*8:(i+1)*8])) for i in range(len(alldata)/8)]

print '#define VIDEO_LENGTH_FRAMES %d'%len(frames)
print '#define VIDEO_LENGTH_BYTES  %d'%len(allbytes)
print '#define VIDEO_LENGTH_BITS   %d'%NBITS
print '#define VIDEO(i)    (pgm_read_byte(&video[(i)]))'
print '#define KEYFRAME(i) (pgm_read_byte(&keyframe[(i)]))'
print 'uint8_t keyframe[] PROGMEM = '+printArray(keyframe)
print 'uint8_t video[] PROGMEM = '+printArray(allbytes)

#median(concatenate(delta_frames))
