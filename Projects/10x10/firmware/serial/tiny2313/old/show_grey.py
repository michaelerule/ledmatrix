#!/usr/bin/env python
import serial,os,sys,time,tty,termios

MAXBYTES = 1000
baudrate = 9600
port     = '/dev/ttyUSB4'


N = 10
colordepth = 2
framebytes = (2*N)
startcode = chr(0x40)

os.system('stty -F %s 9600 cs8 cread'%port)
ser = serial.Serial(port,baudrate,timeout=0)

print 'WAITING FOR SERIAL'
time.sleep(.1)
if not ser.isOpen():
    print 'CONNECTION FAILED'
    sys.exit(0)
print 'SUCCESS'
    
def clear_all():
    for i in range(framebytes*colordepth):
        ser.write(chr(0))
    
def push_frame(data):
    ser.write(startcode)
    ser.write(startcode)
    ser.write(startcode)
    for byte in data:
        ser.write(chr(byte))

def test(r,c):
    frame = int32(zeros((20,)))
    low   = r<5
    index = c*2 + ( 0 if low else 1 )
    byte  = 2**r if r<5 else 2**(r-5)
    frame[index]=byte
    return frame

def random():
    ser.write(startcode)
    ser.write(startcode)
    ser.write(startcode)
    frame = [chr(randint(32)) for i in range(framebytes*colordepth)]
    for ch in frame:
        ser.write(ch)

def prepare_frame(data):
    frame = int32(zeros((20,)))
    

for i in range(1):
    random()

#for i in range(10):
#    for j in range(10):
#        frame = test(i,j)
#        push_frame(frame)

ser.close()
print 'CLOSED'



