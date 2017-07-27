#!/usr/bin/env python
def documentation(s):
	#print s
	pass
	
documentation('''
Goal:
generate streaming information for scrolling marquee 

news headlines
-- bbc
		not yet implemented
-- al jazeera
		Al Jazeera has a news ticker on their main website ( it is the same 
		across all regions ). The headlines are in a hidden element called
		_BreakingContent and delimited by the # character.
		Update: this is apparently dynamically loaded so I will just scrape
		headlines instead. Story summaries currently liv in divs with class
		"indexSummaryText" and so this will be my search key
-- the guardian
		not yet implemented
-- reddit world news
		using json reddit API

stock prices
-- google finance?
-- stock values ( price * shares held )


time
	using the time python module

weather
	not implemented: dynamic content hard to scrape, need better solution

''')

import json
import urllib2
import BeautifulSoup
import sys
import random
import time
import os

from time import gmtime, strftime
from datetime import datetime
from BeautifulSoup import BeautifulStoneSoup

while True:
	os.system('./livestream.py > text')
	time.sleep(int(random.random()*6000/60))


