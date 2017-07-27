#!/usr/bin/env ipython
from pylab import *
from numpy import *
from scipy.signal import *

'''
We want to try to replace the FHT on the spectrum analizer with a collection
of band pass filters. These filters must be fast and numerically stable, 
with good rejection of out-of-band frequencies. Phase distortion is not an 
issue in that we only wish to esimate power in each band.

We are going to pay very close attention to the specifics here so that we 
can accurately predict performance on the microcontroller without 
calibration.

We use timer 1 output compare to periodically trigger ADC with a blocking
interrupt. If all other active interrupts are non-blocking, this leads to 
precisce triggering of the ADC. By using the "compare to counter" mode we 
can finely tune the frequency of analog sampling. A slower, lower priority, 
non blocking interrupt handles grabbing the ADC result and applying any 
filtering. This handler routine must still return before the next ADC is 
triggered.

You may have misconfigured timer1 -- note that the bits needed to stet up
CTC mode on timer 1 are not the same as those used to set CTC mode on timer 
0. You should fix this before proceeding.

Common frequency ranges
27.ASPECT0 -  4,186.01 Piano
20.0000 - 20,000.00 Human hearing
          22,050.00 CD audio nyquist
          11,025.00 Low-fi audio nyquist
           8,000.00 High quality speech
           5,512.50 Lowest quality music
           4,000.00 Low quality speech

We will need to filter incoming audio data. We will need to high-pass at
20Hz and insert enough DC bias to make the signal positive. We will also
need to cut off at least at the nyquist frequency if not earlier.

Any filter can be turned into an online analytic filter by feeding it's 
output to a phase-rotating complex number, which will track the phase. 
This allows you to obtian an online phase and amplitude estimate using
any exisitg filter. 

The direct form 2 is an implementation of the digital biquad filter. It is
a two-level linear recurrence filter which computes output y form input
x using intermediate signal w :

w[t] = a0 x[t] - a1 w[t-1] - a2 w[t-2] 
y[t] = b0 w[t] + b1 w[t-1] + b2 w[t-2]

The filter coefficeint vectors can be computed using the butter function
from scipy.signal. For example, this will generate coefficients for a 
40Hz low-pass filter:

FCUT = 40
[b,a] = butter(2,FCUT/NYQUIST)

To implement a series of band-pass filters, we will use a set of low-pass
filters. Each band will be the difference of sucessive low-pass filters.

20 28 39 54 75 104 144 200 278 386 537 746 1036 1439 2000 2779 3861 5365 7455 10359 14394 20000

20 		28 		39 		54 		75 		104 	144 
200 	278 	386 	537 	746 	1036 	1439 
2000 	2779 	3861 	5365 	7455 	10359 	14394 
20000

'''

ASPECT = 1000

# define system variables and compute effective sampling frequency

SYSCLOCK   = 16000000.0
PRESCALERS = [0,1,8,64,256,1024]
NCHANNELS  = 1
PRESCALE   = 4
OCR1A      = 2
SAMPLE     = SYSCLOCK/PRESCALERS[PRESCALE]/OCR1A/NCHANNELS
NYQUIST    = SAMPLE/2
FS         = SAMPLE

print NYQUIST
print OCR1A/(18000./NYQUIST)

"""
NBANDS = 23
FSTART = 20
FSTOP  = NYQUIST # stopping frequency in Hz
FREQS  = exp(linspace(log(FSTART),log(FSTOP),NBANDS))

# generate a frequency sweep test signal for this configuration

fscan     = []
minf      = 20.0           #Hz
maxf      = NYQUIST        #Hz
time      = 3.0            #s
samples   = int(time*FS)   #frames
phase     = 0.0            #radians
minlambda = 1./minf        #seconds
maxlambda = 1./maxf        #seconds
minlframe = minlambda*FS   #frames
maxlframe = maxlambda*FS   #frames
minrperf  = 2*pi/minlframe #radians/frame
maxrperf  = 2*pi/maxlframe #radians/frame
velocity     = minrperf    #radians/frame
acceleration = (maxrperf-minrperf)/samples #radians/frame^2
for i in range(samples):
	fscan.append(int8(127*cos(phase)))
	phase    += velocity
	velocity += acceleration	

# This spectrogram will serve as a reference. 
# Any implementation based on filters should look similar to this
#specgram(fscan,Fs=FS)


filterCoefficients = array([ butter(2,FCUT/NYQUIST) for FCUT in FREQS ])

# for comparison, filter the signal using python's built-in filtering 
# routine and plot it
filteredFloat = array([lfilter(b,a,fscan) for b,a in filterCoefficients])
subplot(441)
imshow(array(filteredFloat),aspect=ASPECT)

# for sanity check, re-implement floating point filtering, plot, and check
w0 = float32(zeros(NBANDS))
w1 = float32(zeros(NBANDS))
w2 = float32(zeros(NBANDS))
y  = float32(zeros(NBANDS))
filteredFloat2 = []
for sample in fscan:
	for i,(b,a) in enumerate(filterCoefficients):
		w0[i] = a[0]*sample - a[1]*w1[i] - a[2]*w2[i]
		y[i]  = b[0]*w0[i]  + b[1]*w1[i] + b[2]*w2[i]
	filteredFloat2.append(array(y))
	w2,w1,w0 = w1,w0,w2
subplot(442)
imshow(array(filteredFloat2).T,aspect=ASPECT)

# compute signed byte approximations of the filter coefficients
# with 1 = 64 fixed point approximation.
# Low frequencies overflow even using 16 bit representations
discreteFilterCoefficients = int32(filterCoefficients*(2**12)+.5)
w0 = int32(zeros(NBANDS)) 
w1 = int32(zeros(NBANDS)) 
w2 = int32(zeros(NBANDS)) 
y  = int32(zeros(NBANDS)) 
filteredFixed = []
for sample in fscan:
	for i in range(NBANDS):
		b,a = discreteFilterCoefficients[i]
		# signal is encoded in signed byts such that 2^7=128=1
		# coefficients are encoded in signed shorts such that 2^8=256=1
		# multiplying these together makes a fixed point representation wehere 2^15=1
		# we would like to store this back into a signal format where 2^6=64=1
		# so we divide by 2^9=512
		w0[i] = a[0]*sample - a[1]*w1[i] - a[2]*w2[i] >> 12
		y [i] = b[0]*w0[i]  + b[1]*w1[i] + b[2]*w2[i] >> 12 # python is warning of an overflow here -- nice of them
	w2,w1,w0 = w1,w0,w2
	filteredFixed.append(array(y))
subplot(443)
imshow(array(filteredFixed).T,aspect=ASPECT)

# Another approach is to decimate the signal by two in a cascade to keep
# track of low frequency aspects of the signal. Which tier our filter is
# will be determined by log2(NYQUIST/F)
TIERS = int32(log(NYQUIST/FREQS)/log(2)+0.5)
MAXTIER = numpy.max(TIERS)

# first, we will try using exponential moving averages to smooth data
# into the next tier. The frequency rejection is not so good with this
# approach but it requires less memory than sliding boxcars.
'''
# sanity check the tiers of data -- floating point
tiers = float32(zeros(MAXTIER+1))
filteredFloat3 = []
for sample in fscan:
	tiers[0] = sample
	for j in range(MAXTIER):
		tiers[j+1] = (tiers[j+1]+tiers[j])/2
	filteredFloat3.append(array(tiers))
subplot(444)
title('tiered data, floating point')
imshow(array(filteredFloat3).T,aspect=ASPECT*NBANDS/MAXTIER)

# sanity check the tiers of data -- fixed point
tiers = int8(zeros(MAXTIER+1))
filteredFloat3 = []
for sample in fscan:
	tiers[0] = sample
	for j in range(MAXTIER):
		tiers[j+1] = tiers[j+1]/2+tiers[j]/2
	filteredFloat3.append(array(tiers))
subplot(445)
title('tiered data, fixed point')
imshow(array(filteredFloat3).T,aspect=ASPECT*NBANDS/MAXTIER)

tierdata = array(filteredFloat3).T
filtered = []
for i in range(NBANDS):
	tier     = TIERS[i]
	rawdata  = tierdata[tier]
	data     = rawdata[::2**tier]
	b,a      = butter(2,float(FCUT)/(NYQUIST/2**TIER))
	scaled   = interp( range(len(rawdata)), range(len(rawdata))[::2**tier], lfilter(b,a,data) )
	filtered.append(scaled)
subplot(446)
title('filtered tiered data')
imshow(array(filtered),aspect=ASPECT)
'''


'''
# Filter coefficeints change. Now NYQUIST is scaled by the filter tier
tieredFilterCoefficients = array([ butter(2,FCUT/(NYQUIST/2**TIER)) for FCUT,TIER in zip(FREQS,TIERS)])

tieredFilterCoefficients = array([ butter(2,FCUT/(NYQUIST/2**TIER)) for FCUT,TIER in zip(FREQS,TIERS)])

#sanity check: using built in filtering method
filtered = [lfilter(b,a,tierdata[t]) for ((b,a),t) in zip(tieredFilterCoefficients,TIERS)]


# now try to apply filtering to the tiered data using floating point
tiers = float32(zeros(MAXTIER+1))
w0 = float32(zeros(NBANDS))
w1 = float32(zeros(NBANDS))
w2 = float32(zeros(NBANDS))
y  = float32(zeros(NBANDS))
filteredFloat3 = []
for k,sample in enumerate(fscan):
	tiers[0] = sample
	for j in range(MAXTIER):
		tiers[j+1] = tiers[j+1]/2+tiers[j]/2
	for i,(b,a) in enumerate(tieredFilterCoefficients):
		t = TIERS[i]
		if (k%(2**t))==0 : 
			w0[i] = a[0]*tiers[t] - a[1]*w1[i] - a[2]*w2[i]
			y[i]  = b[0]*w0[i]    + b[1]*w1[i] + b[2]*w2[i]
			w2[i],w1[i],w0[i] = w1[i],w0[i],w2[i]
	filteredFloat3.append(array(y))
subplot(446)
imshow(array(filteredFloat3).T,aspect=ASPECT)





# now try to apply filtering to the tiered data using floating point
tiers = float32(zeros(MAXTIER+1))
w0 = float32(zeros(NBANDS))
w1 = float32(zeros(NBANDS))
w2 = float32(zeros(NBANDS))
y  = float32(zeros(NBANDS))
filteredFloat3 = []
for k,sample in enumerate(fscan):
	tiers[0] = sample
	for j in range(MAXTIER):
		tiers[j+1] = tiers[j+1]/2+tiers[j]/2
	for i,(b,a) in enumerate(tieredFilterCoefficients):
		t = TIERS[i]
		if (k%t)==0 : 
			w0[i] = a[0]*tiers[t] - a[1]*w1[i] - a[2]*w2[i]
			y[i]  = b[0]*w0[i]    + b[1]*w1[i] + b[2]*w2[i]
			w2[i],w1[i],w0[i] = w1[i],w0[i],w2[i]
	filteredFloat3.append(array(y))
subplot(446)
imshow(array(filteredFloat3).T,aspect=ASPECT)



'''

"""



