#!/usr/bin/env python
# -*- coding: utf-8 -*-
import codecs
fontfile = 'marquee_font_clear.txt'
mapping = u'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 !.,:;|/\[](){}+-÷×*∙~#@$%^&_=\'"<>?♥▒😃😞😐↑↓←→'
#fontfile = 'marquee_font.txt'
#mapping = u'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890 !.,;:\'"[]()#@$%^&*=+<>|/\\'
chars = None
with open(fontfile,'r') as f:
    data = ''.join(f.readlines())
    chars = [map(eval,s.split()) for s in data.split('-')]
print chars


