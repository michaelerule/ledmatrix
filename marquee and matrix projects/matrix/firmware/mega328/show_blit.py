#!/usr/bin/env python
import serial,os,sys,time,tty,termios
from pylab import *

devs = [s for s in os.listdir('/dev') if 'ttyUSB' in s]

MAXBYTES = 1000
baudrate = 9600
port     = '/dev/'+devs[0]

N = 10
colordepth = 2
framebytes = (2*N)
startcode = chr(0x40)

def startSerial():
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
    print ' '.join(['%02x'%n for n in pieces])
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
    
    

'''
os.system('stty -F %s 9600 cs8 cread'%port)
ser = serial.Serial(port,baudrate,timeout=0)
print 'WAITING FOR SERIAL'
time.sleep(.1)
if not ser.isOpen():
    print 'CONNECTION FAILED'
    sys.exit(0)
print 'SUCCESS'
t1= time.time()
for i in range(200):
    sendFrame([uint32(randint(2**27)) for i in range(N+1)])
ser.close()
print 'CLOSED'
t2 = time.time()
fr = 200/ (t2-t1)
print fr
'''

makeFrame([[randint(8) for i in range(N)] for j in range(N)])
   
ser=startSerial()

#sendFrame(makeFrame([[randint(8) for i in range(N)] for j in range(N)]))
#sendFrame(makeFrame([[min(7,7-j) for i in range(N)] for j in range(N)]))

th = lambda x:1-1/(1+exp((4.5-x)*3))
r = lambda x,y:sqrt((x-4.5)**2+(y-4.5)**2)
f = lambda x,y,h:max(0,min(8,int32(th(r(x,y))*(exp(cos(-h*1.1+2.*r(x,y))+1)*log(8)/2))))

NFRAME = 100

t1= time.time()

for h in range(NFRAME):
    sendFrame(makeFrame([[f(i,j,h) for i in range(N)] for j in range(N)]))
    time.sleep(0.05);

'''
for i in range(N):
    for j in range(N):
        frame = int32(zeros((N,N)))
        frame[i,j] = 1
        sendFrame(makeFrame(frame))
        #time.sleep(.2)
'''
closeSerial(ser)
t2 = time.time()
fr = NFRAME/ (t2-t1)
print fr





