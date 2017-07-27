#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''
devs = [s for s in os.listdir('/dev') if 'ttyUSB' in s or 'usbserial' in s or 'tty.HC-05-DevB-1'==s]
if len(devs)<1:
	print "no serial devices found!"
	sys.exit(0)
port = '/dev/'+devs[0]
'''
#tty.usbserial-FTF3HZCD
# stty -F /dev/tty.usbserial-FTF3HZCD 4800 cs8 cread

port = '/dev/tty.HC-05-DevB-1'
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
