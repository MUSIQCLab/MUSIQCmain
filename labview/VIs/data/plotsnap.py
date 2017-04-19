import pylab
import numpy as np

data = np.loadtxt("C:\Users\RA\Documents\VIs\data\snap.txt")

pylab.plot(data, '-')
pylab.plot(data[1:-1]-data[:-2], '0.5')
pylab.grid(True)
pylab.show()
