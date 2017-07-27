#!/usr/bin/env python
import serial,os,sys,time,tty,termios
from pylab import *
from PIL import Image
import pickle

def startSerial():
    MAXBYTES = 1000
    baudrate = 9600
    devs = [s for s in os.listdir('/dev') if 'ttyUSB' in s]
    print "ports  : %s"%' '.join(map(str,devs))
    print "in use : %s"%(' '.join(map(str,inuse)) if len(inuse) else '(none)')
    print "unused : %s"%' '.join(map(str,(list(set(devs)-inuse))))
    port = list(set(devs)-inuse)[0]
    os.system('stty -F /dev/%s 9600 cs8 cread'%port)
    print 'configd: %s'%port
    ser = serial.Serial('/dev/'+port,baudrate,timeout=0)
    print "opening: %s"%port
    time.sleep(.1)
    if not ser.isOpen():
        print 'CONNECTION FAILED'
        sys.exit(0)
    inuse.add(port)
    print "used   : %s"%(' '.join(map(str,inuse)) if len(inuse) else '(none)')
    print "free   : %s"%' '.join(map(str,(list(set(devs)-inuse))))
    ser.portname = port
    print 'opened : %s'%port
    return ser
    
def closeSerial(ser):
    inuse.remove(ser.portname)
    ser.close()
    print 'CLOSED'

def sendInt(n,ser):
    pieces = []
    for i in range(4):
        pieces.append(n&0x7f)
        n=n>>7
    pieces.reverse()
    pieces[0] = pieces[0] | 0x80
    for p in pieces:
        ser.write(chr(p))

def sendFrame(data,ser):
    data[ 0] = data[0] | 0x40000000
    for n in data:
        sendInt(n,ser)

N = 10
framebytes = (2*N)
inuse = set()

print 'loading movie...',
frames = pickle.load(open('frames.p','rb'))
print 'done'
NFRAME = len(frames)
slines = [None for i in range(4)]


while 1:
    frameq = [int32(list(frames)) for i in range(4)]
    while any(map(len,frameq)):
        for i,q in enumerate(frameq):
            if len(q):
                f = q[0]
                frameq[i] = q[1:]
                try:
                    sendFrame(f,slines[i])
                    time.sleep(0.0113);
                except Exception as e:
                    print 'no connection on %d, attempting to create'%i
                    if slines[i]:
                        closeSerial(slines[i])
                    try:
                        slines[i] = startSerial()
                    except Exception as e:
                        print 'failed'
                    print 'lines  : '+' '.join([s.portname if s else 'none' for s in slines])

for ser in slines:
    closeSerial(ser)





