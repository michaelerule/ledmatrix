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
PRESCALE   = 2
OCR1A      = 55
SAMPLE     = SYSCLOCK/PRESCALERS[PRESCALE]/OCR1A/NCHANNELS
NYQUIST    = SAMPLE/2
FS         = SAMPLE

print NYQUIST
print OCR1A/(18000./NYQUIST)

NBANDS = 23
FSTART = 20
FSTOP  = NYQUIST # stopping frequency in Hz
FREQS  = exp(linspace(log(FSTART),log(FSTOP),NBANDS))

# generate a frequency sweep test signal for this configuration
fscan     = []
minf      = 20.0           #Hz
maxf      = NYQUIST        #Hz
time      = 5.0            #s
samples   = int(time*FS)   #frames
phase     = 0.0            #radians
minlambda = 1./minf        #seconds
maxlambda = 1./maxf        #seconds
minlframe = minlambda*FS   #frames
maxlframe = maxlambda*FS   #frames
minrperf  = 2*pi/minlframe #radians/frame
maxrperf  = 2*pi/maxlframe #radians/frame
velocity     = minrperf    #radians/frame
acceleration = exp((log(maxrperf)-log(minrperf))/samples) #radians/frame^2
for i in range(samples):
	fscan.append(int8(127*cos(phase)))
	phase    += velocity
	velocity *= acceleration	
	

NPLOTSX = 3
NPLOTSY = 3
NPLOT   = 1

specgram(fscan,Fs=FS)



PERIODS       = 1./FREQS # in seconds
FRATE_PERIODS = FS*PERIODS
TAUS          = FRATE_PERIODS
ALPHAS        = 1./TAUS
BETAS         = 1. - ALPHAS
FIXEDP        = 23
INTALPHAS     = int32(ALPHAS*2**FIXEDP)
INTBETAS      = int32(BETAS *2**FIXEDP)

TAUFACTOR     = 32
VARTAUS       = TAUFACTOR*TAUS
VARALPHAS     = 1./VARTAUS
VARBETAS      = 1.-VARALPHAS
INTVARALPHAS  = int32(VARALPHAS*2**FIXEDP)
INTVARBETAS   = int32(VARBETAS *2**FIXEDP)

NPLOT   = 1
filtered = []
y = float32(zeros(NBANDS))
for k,sample in enumerate(fscan):
	y = sample*ALPHAS + y*BETAS
	filtered.append(array(y))
subplot(NPLOTSX,NPLOTSY,NPLOT)
imshow(array(filtered).T,aspect=3000)
NPLOT+=1

subplot(NPLOTSX,NPLOTSY,NPLOT)
NPLOT+=1


filtered = []
v = float32(zeros(NBANDS-1))
y = float32(zeros(NBANDS))
for k,sample in enumerate(fscan):
	y = sample*ALPHAS + y*BETAS
	z = y[1:]-y[:-1]
	variance = z*z;
	v = variance*VARALPHAS[1:] + v*VARBETAS[1:]
	filtered.append(v)
subplot(4,1,1)
q = array(filtered)
imshow(q.T,aspect=3000)
subplot(4,1,2)
imshow(numpy.log(q).T,aspect=3000)
subplot(4,1,3)
q = q[:-1,:]/q[1:,:]
imshow(((q-numpy.mean(q))/numpy.std(q)).T,aspect=3000)
subplot(4,1,4)
q = log1p(q)
imshow(q.T,aspect=3000)



filtered = []
v = float32(zeros(NBANDS-1))
y = float32(zeros(NBANDS))
z = float32(zeros(NBANDS))
for k,sample in enumerate(fscan):
	y = sample*ALPHAS + y*BETAS
	z = y*ALPHAS + y*BETAS
	w = z[1:]-z[:-1]
	variance = w*w;
	v = variance*VARALPHAS[1:] + v*VARBETAS[1:]
	filtered.append(v)
subplot(4,1,1)
q = array(filtered)
imshow(q.T,aspect=3000)
subplot(4,1,2)
imshow(numpy.log(q).T,aspect=3000)
subplot(4,1,3)
q = q[:-1,:]/q[1:,:]
imshow(((q-numpy.mean(q))/numpy.std(q)).T,aspect=3000)
subplot(4,1,4)
q = log1p(q)
imshow(q.T,aspect=3000)





filtered = []
y = int8(zeros(NBANDS))
for k,sample in enumerate(fscan):
	y = sample*INTALPHAS + y*INTBETAS >> FIXEDP
	filtered.append(array(y))
subplot(NPLOTSX,NPLOTSY,NPLOT)
imshow(array(filtered).T,aspect=3000)
NPLOT+=1


filtered = []
v = int8(zeros(NBANDS))
y = uint8(zeros(NBANDS))
for k,sample in enumerate(fscan):
	y = sample*INTALPHAS + y*INTBETAS >> FIXEDP
	variance = y*y >> 6;
	v = variance*INTVARALPHAS + v*INTVARBETAS >> FIXEDP
	filtered.append(array(v))
subplot(NPLOTSX,NPLOTSY,NPLOT)
imshow(log1p(array(filtered).T),aspect=3000)
NPLOT+=1



filtered = []
v = int8(zeros(NBANDS))
y = uint8(zeros(NBANDS))
for k,sample in enumerate(fscan):
	y = sample*INTALPHAS + y*INTBETAS >> FIXEDP
	variance = y*y >> 6;
	v = variance*INTVARALPHAS + v*INTVARBETAS >> FIXEDP
	q = log1p(v)
	filtered.append(q[1:]-q[:-1])
subplot(NPLOTSX,NPLOTSY,NPLOT)
imshow(array(filtered).T,aspect=3000)
NPLOT+=1




