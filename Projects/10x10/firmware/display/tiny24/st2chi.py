#!/usr/bin/env jython

fname = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.,:!? \''

#st = 'v i s s i d \' a r t e  . . . v i s s i  d \' a m o r e : : : :   l i f e'

#st1 = '        t i n i'
#st1 = '        Gccd'
#st1 = '        ri ce'
st1 = '        l i f e'
st2 = '        l i t e'

f=open('pstrings.h','w')
f.write('uint8_t titlestring1[] PROGMEM = {'+','.join(['%d'%(fname.index(c)) for c in st1])+', 255};\n')
f.write('uint8_t titlestring2[] PROGMEM = {'+','.join(['%d'%(fname.index(c)) for c in st2])+', 255};\n')
f.flush()
f.close()



