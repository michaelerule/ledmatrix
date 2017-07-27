#!/usr/bin/env python
# -*- coding: utf-8 -*-
import time,sys,os,random
import codecs
import serial,os,sys,time,tty,termios
import codecs
import datetime
import serial,os,sys,time,tty,termios
from pylab import *

devs = [s for s in os.listdir('/dev') if 'ttyUSB' in s]
print devs

MAXBYTES = 1000
baudrate = 9600
delay    = .08
port     = '/dev/'+devs[0]

execfile('load_font.py')

devs = [s for s in os.listdir('/dev') if 'ttyUSB' in s]
if len(devs)<1:
    print "no serial devices found!"
    sys.exit(0)
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

def sendchar(ch):
    i = mapping.index(ch)
    if i>=0 and i<len(chars):
        print ch,
        chardata = chars[i]+[0,];
        for column in chardata:
            ser.write(chr(column))
            time.sleep(delay)
            #raw_input('press2advance')

def send(lines):
    for ch in lines:
        if ch in mapping:
            sys.stdout.flush()
            sendchar(ch)

def sendtime():
    t = datetime.datetime.now().strftime("%b %d, %Y %H:%M")+' -- '
    send(t)
'''
while 1:
	ser.write(chr(255))
	time.sleep(delay)
	'''
while 1:
	send(u'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 !.,:;|/\[](){}+-÷×*∙~#@$%^&_=\'"<>?♥▒😃😞😐↑↓←→')
	sendtime()

for i in range(8):
	ser.write(chr(2**i))
	#//time.sleep(delay)
for i in range(8):
	ser.write(chr(2**i))
	time.sleep(delay)
	#sendtime()

closeSerial(ser)

