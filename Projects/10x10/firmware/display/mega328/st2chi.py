#!/usr/bin/env jython

fname = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.,:!? \''

#st = 'v i s s i d \' a r t e  . . . v i s s i  d \' a m o r e : : : :   l i f e'

st0 = '   I '
st1 = ' L o v e '
st2 = ' Y O U '
st0 = 'G n i t e'
st1 = ' S l eep '
st2 = ' W e l l  '
st0 = 'G o o d '
st1 = 'n i g h t'
st2 = '   '

f=open('pstrings.h','w')
f.write('uint8_t titlestring0[] PROGMEM = {'+','.join(['%d'%(fname.index(c)) for c in st0])+', 255};\n')
f.write('uint8_t titlestring1[] PROGMEM = {'+','.join(['%d'%(fname.index(c)) for c in st1])+', 255};\n')
f.write('uint8_t titlestring2[] PROGMEM = {'+','.join(['%d'%(fname.index(c)) for c in st2])+', 255};\n')
f.flush()
f.close()



