#!/usr/bin/env python
import serial,os,sys,time,tty,termios

MAXBYTES = 1000
baudrate = 9600
port     = '/dev/ttyUSB1'

os.system('stty -F %s 9600 cs8 cread'%port)
ser = serial.Serial(port,baudrate,timeout=0)
print 'WAITING FOR SERIAL'
time.sleep(.1)
if not ser.isOpen():
    print 'CONNECTION FAILED'
    sys.exit(0)
print 'SUCCESS'
    
def clear_all():
    for i in range(21):
        ser.write(chr(0x0))
    ser.write(chr(0x40))
    for i in range(21):
        ser.write(chr(0x0))
    
def push_frame(data):
    ser.write(chr(0x80))
    ser.write(chr(0x80))
    for byte in data:
        ser.write(chr(byte))
    ser.write(chr(0x40))

def test(r,c):
    frame = int32(zeros((20,)))
    low   = r<5
    index = c*2 + ( 0 if low else 1 )
    byte  = 2**r if r<5 else 2**(r-5)
    frame[index]=byte
    return frame

for i in range(22):
    clear_all()
for i in range(10):
    for j in range(10):
        frame = test(i,j)
        push_frame(frame)

ser.close()
print 'CLOSED'



