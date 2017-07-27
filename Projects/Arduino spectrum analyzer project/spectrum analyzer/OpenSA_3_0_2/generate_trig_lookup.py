#!/usr/bin/env python

from math import *

Nin  = 32;
Nout = 15;

print [int((cos(i*2*pi/Nin)+1)*Nout/2) for i in range(Nin)]
