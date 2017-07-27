#!/usr/bin/env python
import serial,os,sys,time,tty,termios

MAXBYTES = 1000
baudrate = 9600
port     = '/dev/ttyUSB1'

ser = serial.Serial(port,baudrate,timeout=0)
print 'WAITING FOR SERIAL'
time.sleep(.1)
if not ser.isOpen():
    print 'CONNECTION FAILED'
    sys.exit(0)
print 'SUCCESS'

def getChar():
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(sys.stdin.fileno())
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    return ch

char = getChar()
while char!='q':
    ser.write(char)
    # listen for echo
    for i in range(10000):
        data = ser.read(MAXBYTES)
        if len(data)>0:
            sys.stdout.write(data)
            sys.stdout.flush()
            break
    char = getChar()
    
ser.close()
print 'CLOSED'

