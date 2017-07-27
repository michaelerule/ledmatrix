#!/usr/bin/env python
import serial,os,sys,time,tty,termios
from pylab import *

devs = [s for s in os.listdir('/dev') if 'ttyUSB' in s]

port     = '/dev/'+devs[0]

def startSerial(baudrate = 9600):
    os.system('stty -F %s %d cs8 cread'%(port,baudrate))
    ser = serial.Serial(port,baudrate,timeout=0)
    time.sleep(.1)
    if not ser.isOpen():
        print 'CONNECTION FAILED'
        sys.exit(0)
    print 'SUCCESS, USING PORT %s AT %d'%(port,baudrate)
    return ser
    
def closeSerial(ser):
    ser.close()
    print 'CLOSED'


#for baudrate in [2400,4800,9600,19200,38400,57600,115200,31250]:
#	print baudrate,baudrate*4,1000000/(baudrate*4)

baudrate = 4800

ser=startSerial(baudrate)

delay = 0.05

sequence = [ 0,0,0xff,0xa0,0x80,0x38,0x08,0x38,0x0,0x39,0x28,0x28,0x0,0xff,0x10,0x28,0,0]

for i in sequence:
	k = 0;
	for j in range(3,8):
		k = (k<<1)|((i>>j)&1)
	ser.write(chr(k))
	time.sleep(delay)

closeSerial(ser)




