#!/usr/bin/env python
import serial,os,sys,time,tty,termios
from pylab import *
from PIL import Image

N = 10
framebytes = (2*N)

def startSerial():
    MAXBYTES = 1000
    baudrate = 9600
    devs = [s for s in os.listdir('/dev') if 'ttyUSB' in s]
    port = '/dev/'+devs[randint(len(devs))]
    print "sending on port %s"%port
    os.system('stty -F %s 9600 cs8 cread'%port)
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

def sendInt(n):
    pieces = []
    for i in range(4):
        pieces.append(n&0x7f)
        n=n>>7
    pieces.reverse()
    pieces[0] = pieces[0] | 0x80
    for p in pieces:
        ser.write(chr(p))

def sendFrame(data):
    data[ 0] = data[0] | 0x40000000
    for n in data:
        sendInt(n)

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
    data = int32(exp(float32(list(Image.open('./video/'+f).getdata())).reshape((10,10))/110.))-1
    return makeFrame(data)

#frames = map(getFrame,sorted([s for s in os.listdir('./video') if '.png' in s]))
import pickle
frames = pickle.load(open('frames.p','rb'))
  
for i in range(150):
    try:
        ser = startSerial()
        break
    except:
        print 'FAILED, ATTEMPT %d'%i

NFRAME = len(frames)
t1= time.time()
for f in frames:
    try:
        sendFrame(f)
        time.sleep(0.049);
    except:
        print 'LOST CONNECTION, ATTEMPTING TO RESUME'
        closeSerial(ser)
        for i in range(150):
            try:
                ser = startSerial()
                break
            except:
                print 'FAILED, ATTEMPT %d'%i
        pass

closeSerial(ser)
t2 = time.time()
fr = NFRAME/ (t2-t1)
print fr





