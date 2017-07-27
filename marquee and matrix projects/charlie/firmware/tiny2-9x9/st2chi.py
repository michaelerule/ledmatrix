#!/usr/bin/env jython

fname = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890., ;:-+=?!\'*'
st1 = '        l i f e'
st2 = '        l i t e'

f=open('pstrings.h','w')
f.write('uint8_t titlestring1[] PROGMEM = {'+','.join(['%d'%(fname.index(c)) for c in st1])+', 255};\n')
f.write('uint8_t titlestring2[] PROGMEM = {'+','.join(['%d'%(fname.index(c)) for c in st2])+', 255};\n')
f.flush()
f.close()



