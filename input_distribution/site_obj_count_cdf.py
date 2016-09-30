import matplotlib.pyplot as plt 
import numpy

params = {
   'axes.labelsize': 10,
   'font.size': 10,
   'legend.fontsize': 10,
   'xtick.labelsize': 8,
   'ytick.labelsize': 8,
   'text.usetex': False,
   'figure.figsize': [4.5, 3.5]
   }
plt.rcParams.update(params)

data =[]
for line in open('all_objcounts'):   
 data.extend([float(val) for val in line.split()]) 
data2=numpy.sort(data)
y2 = numpy.arange( len(data2)*1.0)/len(data2)

data =[]
for line in open('all_objcount_todays_cite'):   
 data.extend([float(val) for val in line.split()]) 
data3=numpy.sort(data)
y3 = numpy.arange( len(data3)*1.0)/len(data3)

plt.plot(data2,y2,'b-',label="site 2014",linewidth=2)
plt.plot(data3,y3,'r-',label="site today",linewidth=2)
plt.xlabel("Number of objects")
plt.ylabel("CDF")
plt.legend(frameon=False)
plt.savefig('obj_count_comparison.pdf',format='pdf')

