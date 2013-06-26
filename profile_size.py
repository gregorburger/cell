import os
import subprocess as sp
import locale
import matplotlib.pyplot as plt

encoding = locale.getdefaultlocale()[1]

os.environ["LD_PRELOAD"]="/usr/lib/libllalloc.so.1.3"

complete = []
nps = []

for np in [10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1600, 1800, 2000, 2200, 2400, 2600, 2800]:
	
	p = sp.Popen(["./cell", "%s" % np], stdout=sp.PIPE)
	#p.wait()
	output = p.communicate()[0].decode(encoding).strip()
	print("finished")
	lines = output.split('\n')
	b = lines[4].split()[3]
	f = lines[5].split()[3]
	c = lines[6].split()[3]
	
	complete.append(float(c))
	nps.append(np)
	

from scipy.optimize import curve_fit
import numpy 

def lin_func(x, k, d):
	return k*x+d

nps = numpy.array([np*np for np in nps])
complete = numpy.array(complete)

print nps
print complete

print type(nps)

popt, popc = curve_fit(lin_func, nps, complete)
print popt

plt.figure()
plt.plot(nps, complete)
plt.plot(nps, lin_func(nps, popt[0], popt[1]))
plt.show()
