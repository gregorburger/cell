import os
import subprocess as sp
import locale
import matplotlib.pyplot as plt

encoding = locale.getdefaultlocale()[1]

os.environ["LD_PRELOAD"]="/usr/lib/libllalloc.so.1.3"

nps = []
building = []
finding = []
complete = []
speedup_b = []
speedup_f = []
speedup_c = []

for np in [1, 2, 3, 4, 5, 6, 7, 8]:
	os.environ["OMP_NUM_THREADS"] = "%s" % np
	p = sp.Popen("./cell", stdout=sp.PIPE)
	#p.wait()
	output = p.communicate()[0].decode(encoding).strip()
	print("finished")
	lines = output.split('\n')
	b = lines[4].split()[3]
	f = lines[5].split()[3]
	c = lines[6].split()[3]
	building.append(b)
	finding.append(f)
	complete.append(c)
	nps.append(np)
	if np == 1:
		speedup_f.append(1)
		speedup_b.append(1)
		speedup_c.append(1)
	else:
		speedup_b.append(float(building[0]) / float(b))
		speedup_f.append(float(finding[0]) / float(f))
		speedup_c.append(float(complete[0]) / float(c))

plt.figure()
plt.plot(range(1,9), finding)
plt.plot(range(1,9), building)
plt.plot(range(1,9), complete)
plt.figure()
plt.plot(range(1,9), speedup_f)
plt.plot(range(1,9), speedup_b)
plt.plot(range(1,9), speedup_c)
plt.plot(range(1,9), [1,2,3,4,5,6,7,8])
plt.show()
