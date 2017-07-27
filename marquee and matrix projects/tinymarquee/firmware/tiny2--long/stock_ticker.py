#!/usr/bin/env python
# -*- coding: utf-8 -*-

def documentation(s):
	#print s
	pass

import json
import urllib2
import BeautifulSoup
import sys

from time import gmtime, strftime
from datetime import datetime
from BeautifulSoup import BeautifulStoneSoup

from random import randint

import serial,os,sys,time,tty,termios
from pylab import *

execfile('./ystockquote.py')

baudrate = 4800
delay = 0.06

mapping = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890., ;:-+=?!\'*^$&'
stocks = '''
S&P500
DOW
VEIEX
VGPMX
VFORX
VWINX
'''

def getPrices(stocks):
	'''
	Stock monitoring
	'''	
	prices = []
	stocks = stocks.split('\n')[1:-1]
	for name in stocks:
		#shouldn't be using float for finance, change this if things get serious
		price = float(get_price(name))
		prices.append("%s %s"%(name,price))
	return prices

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

def scrollString(ser,stringdata):
	for char in stringdata:
		i = mapping.index(char)
		chardata = chars[i]+[0,];
		for column in chardata:	
			ser.write(chr(column))
			time.sleep(delay)

'''
Load bitmap font from processed file
( run png2int to regenerate )
'''
chars = None
with open('alphadata.h','r') as f:
	data = ''.join(f.readlines())
	chars = [map(eval,s.split()) for s in data.split('-')]

'''
Look for a COM port to use -- override if you have somethig specific
'''
devs = [s for s in os.listdir('/dev') if 'ttyUSB' in s]
port = '/dev/'+devs[0]

everysooften=0
ser=startSerial(baudrate)
while 1:
	stringdata = '      Time: '+strftime("%Y-%m-%d %H:%M:%S")+'         '
	print stringdata
	scrollString(ser,stringdata)
	
	if everysooften==0:
		stockdata = '      '+' -- '.join(getPrices(stocks))+'         '
		print stockdata
		everysooften = 500 + randint(0,1000)
	everysooften = everysooften-1
	
	scrollString(ser,stockdata)
closeSerial(ser)


