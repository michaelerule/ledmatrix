#!/usr/bin/env ipython
# -*- coding: utf-8 -*-
import os,sys
import serial,os,sys,time,tty,termios,fcntl

port     = '/dev/ttyUSB0'
MAXBYTES = 1000
baudrate = 9600

def getch():
  fd = sys.stdin.fileno()
  oldterm = termios.tcgetattr(fd)
  newattr = termios.tcgetattr(fd)
  newattr[3] = newattr[3] & ~termios.ICANON & ~termios.ECHO
  termios.tcsetattr(fd, termios.TCSANOW, newattr)
  oldflags = fcntl.fcntl(fd, fcntl.F_GETFL)
  fcntl.fcntl(fd, fcntl.F_SETFL, oldflags | os.O_NONBLOCK)
  try:        
    while 1:            
      try:
        c = sys.stdin.read(1)
        break
      except IOError: pass
  finally:
    termios.tcsetattr(fd, termios.TCSAFLUSH, oldterm)
    fcntl.fcntl(fd, fcntl.F_SETFL, oldflags)
  return c

def startSerial(baudrate):
	os.system('stty -F %s %d cs8 cread'%(port,baudrate))
	ser = serial.Serial(port,baudrate,timeout=0)
	time.sleep(.1)
	if not ser.isOpen():
	    print 'CONNECTION FAILED'
	    sys.exit(0)
	print 'SUCCESS'
	return ser
	
def closeSerial(ser):
	ser.close()
	print 'CLOSED'
			
ser = startSerial(baudrate)
#for i in range(100):
while 1:
	ser.write(getch())
closeSerial(ser)







