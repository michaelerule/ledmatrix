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

from time import gmtime, strftime
from datetime import datetime
from BeautifulSoup import BeautifulStoneSoup

execfile('./ystockquote.py')

headlines = set()

#create a custom user agent string to conform to reddit API TOS
opener = urllib2.build_opener()
#opener.addheaders = [('User-agent', 'headline_scraper michaelerule')]
# Just kidding lets pretend to be chrome :P
opener.addheaders = [('User-agent','Mozilla/5.0 (Windows NT 6.0; WOW64) AppleWebKit/534.24 (KHTML, like Gecko) Chrome/11.0.696.16 Safari/534.24')]
urlopen = opener.open


documentation('''
Scrape Al Jazeera Headlines
''')
stem    = 'http://www.aljazeera.com/'
tocrawl = [stem]+[stem+'news/'+rest for rest in 'africa americas asia-pacific asia europe middleeast'.split()]
for site in tocrawl:
	for soup in BeautifulStoneSoup(urlopen(site).read()).findAll('td'):
		for a in soup.findAll('a', attrs={'class':'indexText-Bold2 indexText-Font2'}):
			if len(a):
				trim = ' '.join(a.contents[0].split())
				if trim:
					headlines.add(trim)

documentation('''
Scrape reddit headlines
''')
subreddits = 'worldnews news'.split()
for subreddit in subreddits:
	for child in json.load(urlopen("http://www.reddit.com/r/%s/top/.json"%subreddit))['data']['children']:
		trim = ' '.join(child['data']['title'].split())
		if trim:
			headlines.add(trim)

documentation('''
Done scraping, headlines are:
''')
headlines = [h for h in headlines if len(h)<60] # enforce 60 char limit -- some reddit titles are long
for headline in headlines:
	print headline

documentation('''
Timezone information
''')
print 'Time: '+strftime("%Y-%m-%d %H:%M:%S")
#print 'GMT: '+strftime("%Y-%m-%d %H:%M:%S", gmtime())

documentation('''
Weather forecast
''')
locations = {
	'Providence':(41.8239, 71.4133,'Pvd'),
	'Boston':(42.3583, 71.0603,'Bos'),
	'Baltimore':(39.2833, 76.6167,'Btm'),
	'Pittsburgh':(40.4406, 79.9961,'Pgh'),
	'Los Angeles':(34.0522, 118.2428,'LA')}
'''
for loc,(lat,lon,abbrv) in locations.iteritems():
	documentation('Weather info for %s'%loc)
	site = 'http://mobile.weather.gov/index.php?lat=%s&lon=%s'%(lat,lon)
	soup = BeautifulSoup.BeautifulSoup(urlopen(site).read())
	for d in soup.findAll('div'):
		try:
			if d['class']=='ui-bar ui-bar-c':
				print d.content
				print d
		except:
			pass
'''

documentation('''
Stock monitoring
''')
stocks = '''
S&P500
DOW
'''
stocks = stocks.split('\n')[1:-1]
for name in stocks:
	#shouldn't be using float for finance, change this if things get serious
	price = float(get_price(name))
	print "%s %s"%(name,price)



