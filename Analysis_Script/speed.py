import numpy as np
import scipy.stats as st
import matplotlib.pyplot as plt
from collections import Counter
import pandas as pd
from statistics import mean,variance

#sizes = [8, 16, 32, 64, 128, 256, 512]  # kb level
sizes = [1,2,4,8,16,32,64,128,256,512] #Mb level
#sizes = [1,2,4,8] #GB level

DATA_LENGTH = len(sizes)

MM = [None]*DATA_LENGTH
MA = [None]*DATA_LENGTH
MM_value = [None]*DATA_LENGTH
MA_value = [None]*DATA_LENGTH
MM_average = [None]*DATA_LENGTH
MA_average = [None]*DATA_LENGTH
MM_variance = [None]*DATA_LENGTH
MA_variance = [None]*DATA_LENGTH
j = 0

for i in sizes:
    MA[j] = open(
        "results/size/M/%s/0.txt" % i, "r")
    MM[j] = open(
        "results/size/M/%s/1.txt" % i, "r")
    j = j+1


def dataread(file):
    # Repeat for each song in the text file
    #    i = 0
    time = []
    for line in file:

        fields = line.split(",")

        time.append(float(fields[1]))

    file.close()
    # realloc_counter_np = np.array(realloc_counter)
    # copy_percentage_np = np.array(copy_percentage)

    return time
# yerr=np.var(time1)


for j in range(DATA_LENGTH):
    MA_value[j] = dataread(MA[j])
    MA_average[j] = round(mean(MA_value[j]), 2)
    MA_variance[j] = round(variance(MA_value[j]), 2)
    MM_value[j] = dataread(MM[j])
    MM_average[j] = round(mean(MM_value[j]), 2)
    MM_variance[j] = round(variance(MM_value[j]), 2)

# print(MA_average)
# print(MM_average)

# plt.plot(MA_average, marker='o', label='Realloc',yerr = MA_variance)
# plt.plot(MM_average, marker='o', label='Mremmap',yerr = MM_variance)

fig, ax = plt.subplots()
ax.boxplot((MM_value[0], MM_value[1],MM_value[2], MM_value[3],MM_value[4], MM_value[5],MM_value[6],MM_value[7], MM_value[8],MM_value[9]), vert=True, showmeans=True, meanline=True,
           labels=('1','2','4','8', '16','32', '64','128', '256','512'), patch_artist=True,
           medianprops={'linewidth': 2, 'color': 'purple'},
           meanprops={'linewidth': 2, 'color': 'red'})

#plt.xticks([0,1, 2, 3, 4,5,6,7], ['8','16', '32', '64', '128','256','512'])
# plt.xticks([0, 1, 2, 3, 4, 5, 6, 7, 8, 9], ['1', '2', '4',
#            '8', '16', '32', '64', '128', '256', '512'])
# plt.xticks([0,1, 2, 3], ['1','2', '4', '8'])
plt.ylim(ymin=0.0)
plt.legend()

plt.title("Mremap speed with MB data size")
plt.ylabel("time (ms)")
plt.xlabel("Data size (MB)")

figure_name = "mBmremap.png"
plt.savefig(figure_name, format='png', bbox_inches='tight')

plt.show()
