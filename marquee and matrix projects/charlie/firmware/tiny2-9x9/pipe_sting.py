#!/usr/bin/env python
# -*- coding: utf-8 -*-
import serial,os,sys,time,tty,termios
from pylab import *

baudrate = 4800
delay = 0.1
test = "Also... fuck"
#test = "I & U Olivia"

test = """
S&P500 5.69
DOW 32.59
"""

test = ' - '.join(test.split('\n'))
print test

mapping = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890., ;:-+=?!\'*^$&'

for c in test:
	print c
	print mapping.index(c)

indexes = [mapping.index(c) for c in test]

chars = None
with open('alphadata.h','r') as f:
	data = ''.join(f.readlines())
	chars = [map(eval,s.split()) for s in data.split('-')]
print chars

devs = [s for s in os.listdir('/dev') if 'ttyUSB' in s]
port = '/dev/'+devs[0]

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

ser=startSerial(baudrate)

for i in indexes:
	chardata = chars[i]+[0,];
	for column in chardata:	
	#	k = 0;
	#	for j in range(3,8):
	#		k = (k<<1)|((i>>j)&1)
		ser.write(chr(column))
		time.sleep(delay)

closeSerial(ser)





