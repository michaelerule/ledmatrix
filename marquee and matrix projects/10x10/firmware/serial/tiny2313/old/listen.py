#!/usr/bin/env python
import serial,os,sys,time

MAXBYTES = 1000
baudrate = 9600
port     = '/dev/ttyUSB1'

ser = serial.Serial(port,baudrate,timeout=0)

print 'WAITING FOR SERIAL'
time.sleep(1)
if not ser.isOpen():
    print 'CONNECTION FAILED'
    sys.exit(0)
print 'SUCCESS'

for i in range(10000):
    data = ser.read(MAXBYTES)
    if len(data) > 0:
        print data,
    
ser.close()
print 'CLOSED'

