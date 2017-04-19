import numpy as np
from matplotlib import pyplot as plt

#data = np.loadtxt('hv_brick_test1.txt')
data = np.loadtxt('pockel-tx-vs-voltage.txt')
times = data[:,0]
pd = data[:,2]
ht = data[:,1]*1000

plt.ylabel('Photodiode signal')
plt.xlabel('Applied Voltage (V)')
plt.plot(ht, pd, 'b.')
plt.grid(True)
plt.show()