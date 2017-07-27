#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

while 1:
	sys.stdout.write('type something: ')
	sys.stdout.flush()
	line = raw_input()
	f = open('linedata','w')
	f.write(line)
	f.flush()
	f.close()



