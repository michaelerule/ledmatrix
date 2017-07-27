#!/usr/bin/env python
import serial,os,sys,tty,time,random

bluetooth = '/dev/tty.HC-05-DevB-1'
ttlserial = '/dev/tty.usbserial-FTF3HZCD'

baudrate = 4800
baudrate = 38400
baudrate = 19200
baudrate = 57600
baudrate = 115200
baudrate = 9600

print 'ATTEMPTING TO OPEN BLUETOOTH CONNECTION'
#os.system('stty -f %s %d cs8 cread'%(bluetooth,baudrate))
bser = serial.Serial(bluetooth,baudrate,timeout=0.1)
time.sleep(.1)
if not bser.isOpen():
    print 'CONNECTION BLUETOOTH FAILED'
    sys.exit(0)
print 'SUCCESS, USING PORT %s AT %d BLUETOOTH'%(bluetooth,baudrate)

for i in range(50):
    #bser.write(chr(int(random.random()*255)))
    bser.write('1')
    time.sleep(0.5)
    bser.write('0')
    time.sleep(0.5)
    bser.write(chr(1))
    time.sleep(0.5)
    bser.write(chr(0))
    time.sleep(0.5)
    #b=bser.read()
    #print '%x BLU %s'%(ord('B'),'%x'%ord(b) if b else None)
    #time.sleep(.1)
    #print 'BLUETOOTH RATE is %s'%bser.baudrate

bser.close()
print 'CLOSED'
