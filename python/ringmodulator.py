# This file is just a test bed to mess around with ring modulation to get a better understanding

import numpy as np
import matplotlib.pyplot as plt
import scipy.fftpack

# Number of samplepoints
N = 192000
# sample spacing
T = 1.0 / N
x = np.linspace(0.0, N*T, N)
y = ( np.sin(10000.0 * 2.0*np.pi*x) + 0.5*np.sin(11000.0 * 2.0*np.pi*x) ) * np.sin(8000.0 * 2.0*np.pi*x)
yf = scipy.fftpack.fft(y)
xf = np.linspace(0, 1.0/(2.0*T), N/2)

fig, ax = plt.subplots()
ax.plot(xf, 2.0/N * np.abs(yf[:N//2]))
plt.show()
