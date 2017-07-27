#!/usr/bin/env python
# -*- coding: utf-8 -*-
import codecs

fontfile = 'marquee_font.txt'
# Load the font information
mapping = u'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 !.,:;|/\[](){}+-÷×*∙~#@$%^&_=\'"<>?♥▒😃😞😐↑↓←→'
chars = None
with open(fontfile,'r') as f:
    data = ''.join(f.readlines())
    chars = [map(eval,s.split()) for s in data.split('-')]
print chars


