#!/usr/bin/env python
# -*- coding: utf-8 -*-
import time,sys,os,random
import codecs
import serial,os,sys,time,tty,termios
import codecs
import datetime
import serial,os,sys,time,tty,termios
import time,sys,os,random
import codecs
import serial,os,sys,time,tty,termios
import codecs
import datetime
import random
import termios
import fcntl

MAXBYTES = 1000
baudrate = 9600
delay    = .08
newsfile = 'news'
stockfile = 'stocks'
papers = 'papers'
baudrate = 9600#115200#38400#4800 hmm 115200
delay = 0.061

#port = '/dev/tty.HC-05-DevB-1'
port = '/dev/ttyUSB0'

def startSerial(baudrate = 9600):
    os.system('stty -f %s %d cs8 cread'%(port,baudrate))
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

kk = 0
delay = [delay]

def sendchar(ch):
    global kk
    i = mapping.index(ch)
    if i>=0 and i<len(chars):
        chardata = chars[i]+[0,];
        for column in chardata:
            ser.write(chr(column))
            time.sleep(delay[kk])
            kk+=1
            if kk>=len(delay): kk=0

def send(lines):
    for ch in lines:
        if ch in mapping:
            sendchar(ch)

print 'attempting to open serial'
ser=startSerial(baudrate)
try:
	execfile('load_font.py')
	while 1:
		f = open('linedata','r')
		for l in f.readlines():
			l = ' '.join(l.split())
			l = ' '*8+l
			send(l)
except:
	'that was bad'
finally:
	closeSerial(ser)



