#!/usr/bin/env python
import os,time
os.system('./bluetooth.py &')
time.sleep(0.2)
os.system('./ttl.py &')

