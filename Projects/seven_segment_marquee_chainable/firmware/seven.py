#!/usr/bin/env ipython

upper = 'ABCEFGHIJLOPQRSTUYZ-'
lower = 'abcdfghijlnopqrtu-'

upper = set(upper.lower())
lower = set(lower)

onlyUpper = upper-lower
onlyLower = lower-upper

valid = upper|lower

def clean(s):
	s = s.lower().strip()
	n = []
	for c in s:
		if not c in 'qwertyuiopasdfghjklzxcvbnm-,.?;:"\' \t\n\r':
			return ''
		if c in 'qwertyuiopasdfghjklzxcvbnm-':
			n.append(c)
	z = ''.join(n)
	return z

def can7(s):
	s = clean(s)
	if not all([c in valid for c in s]):
		return 0
	# all caps is ok
	if all([c in upper for c in s]):
		if not 'i' in s[1:-1]:
			return 1
		# special heuristics for half-width characters
		# if it is upper case, I is the only half width char
		# it can appear at the beginning, end, or repeated, but is otherwise
		# not allowed. However, half width chars located at the end can 
		# be either in isolation or in a pair. 
		if s[0]=='i':
			if not 'i' in s[1:].replace('ii','k'):
				return 1
		if s[-1]=='i':
			if not 'i' in s[:-1].replace('ii','k'):
				return 1
		return not 'i' in s.replace('ii','k')
	# all lower or title-case is ok
	if all([c in lower for c in s[1:]]):
		z = s.replace('l','i')
		if not 'i' in z[1:-1]:
			return 1
		# special heuristics for half-width characters
		# if it is lower case, i and l are half-width chars
		# it can appear at the beginning, end, or repeated, but is otherwise
		# not allowed
		if z[0]=='i':
			if not 'i' in z[1:].replace('ii','k'):
				return 1
		if z[-1]=='i':
			if not 'i' in z[:-1].replace('ii','k'):
				return 1
		return not 'i' in z.replace('ii','k')
	return 0

def to7(s):
	s = clean(s)
	if not can7(s):
		return None
	# prefer title-case or lower case
	if all([c in lower for c in s[1:]]):
		if s[0] in upper:
			return s[:1].upper()+s[1:]
		return s
	return s.upper()

'''
ok = set()
words = '/usr/share/dict/american-english'
for l in open(words,'r').readlines():
	if not "'" in l and can7(l):
		ok.add(l[:-1])
words = '/usr/share/dict/british-english'
for l in open(words,'r').readlines():
	if not "'" in l and can7(l):
		ok.add(l[:-1])
words = '/home/mrule/Desktop/Dropbox/Public/bloggerscripts/wealonecorpus.txt'
for l in open(words,'r').readlines():
	for w in l.split():
		w = clean(w)
		if len(w)>0 and not "'" in w and can7(w):
			ok.add(w)

print ' '.join(map(to7,sorted(list(ok))))
'''




