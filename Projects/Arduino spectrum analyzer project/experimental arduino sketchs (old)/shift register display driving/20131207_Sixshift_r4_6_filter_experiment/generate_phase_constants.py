#!/usr/bin/env ipython
from pylab import *

NBANDS     = 18
SAMPLERATE = 15625/4.
lowfreq    = 30.
highfreq   = SAMPLERATE/2.
freqs      = exp(linspace(log(lowfreq),log(highfreq),NBANDS))

#print freqs

logtable = [int((0.5*log1p(x/127.))/(0.5*log1p(1))*255) for x in range(128)]
print 'const uint8_t LOGTABLE[128] = {%s};'%','.join(map(str,logtable))

IMIN = -32768
IMAX = 32767
ISCALE = IMAX-1
# theta is such that two-pi rotations is one wavelength
# so we get the number of samples in one wavelength and select theta
# such that one complete phase rotation is equal to that many samples
# that is to say
# omega = 2*PI/nsamples = 2*PI*f
fcoef = []
for i,f in enumerate(freqs):
	nsamples = SAMPLERATE/f
	omega = 2*pi/nsamples
	alpha = exp(-omega*0.001)
	z = alpha*exp(1j*omega)
	a = z.real
	b = z.imag
	a = int(a*ISCALE+0.5)
	b = int(b*ISCALE+0.5)
	#a = 0 if a<0 else 255 if a>255 else a
	#b = 0 if b<0 else 255 if b>255 else b
	a = IMIN if a<IMIN else IMAX if a>IMAX else a
	b = IMIN if b<IMIN else IMAX if b>IMAX else b
	#print a,b
	fcoef.append(a)
	fcoef.append(b)
	

print 'const int16_t FCOEF[NBANDS*4] = {%s};'%','.join(map(str,fcoef))


fcoef = []
for i,f in enumerate(freqs):
	nsamples = SAMPLERATE/f
	omega = 2*pi/nsamples
	fcoef.append(int(omega*128/pi))

print 'const uint8_t phase_velocities[NBANDS] = {%s};'%','.join(map(str,fcoef))

fcoef = []
for i,f in enumerate(freqs):
	nsamples = SAMPLERATE/f
	omega = 2*pi/nsamples
	alpha = exp(-omega)
	fcoef.append(int(alpha*256))
print 'const uint8_t decay_rates[NBANDS] = {%s};'%','.join(map(str,fcoef))



fcoef = [ int(sin(x*2*pi/256)*127) for x in range(256) ]
print 'const int8_t sine_table[256] = {%s};'%','.join(map(str,fcoef))
fcoef = [ int(cos(x*2*pi/256)*127) for x in range(256) ]
print 'const int8_t cos_table[256] = {%s};'%','.join(map(str,fcoef))











