#!/usr/bin/env python
import serial,os,sys,tty,time

bluetooth = '/dev/tty.HC-05-DevB-1'
ttlserial = '/dev/tty.usbserial-FTF3HZCD'

baudrate = 4800
baudrate = 38400
baudrate = 19200
baudrate = 57600
baudrate = 115200

print 'ATTEMPTING TO OPEN TTL SERIAL CONNECTION'
os.system('stty -f %s %d cs8 cread'%(ttlserial,baudrate))
tser = serial.Serial(ttlserial,baudrate,timeout=0.1)
time.sleep(.1)
if not tser.isOpen():
    print 'CONNECTION TTL FAILED'
    sys.exit(0)
print 'SUCCESS, USING PORT %s AT %d TTL'%(ttlserial,baudrate)

for i in range(50):
    tser.write('T')
    t=tser.read()
    if t:
        sys.stdout.write(t)
        sys.stdout.flush()
    #print '%x TTL %s'%(ord('T'),'%x'%ord(t) if t else None)
    time.sleep(.1)
    #print 'TTL RATE is %s'%tser.baudrate

tser.close()
print 'CLOSED'
