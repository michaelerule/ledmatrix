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
import time
import sys
import os

os.system('reset')

slideaway = ' '*80

while 1:
	with open('./text') as myfile:
		lines = myfile.readlines()
		lines = ' - - - '.join(lines)
		lines = ''.join(lines.split('\n'))
		length = len(lines)
		for c in lines:
			slideaway = slideaway[1:]+c
			sys.stdout.write('\b'*80+slideaway)
			sys.stdout.flush()
			time.sleep(0.1)
		
		
