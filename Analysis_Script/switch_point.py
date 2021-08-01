import numpy as np
import scipy.stats as st
import matplotlib.pyplot as plt
from collections import Counter
import pandas as pd
from statistics import mean,variance,median

M1 = open("8.txt", "r")
M2 = open("16.txt", "r")
M3 = open("32.txt", "r")
M4 = open("64.txt", "r")
M5 = open("128.txt", "r")
M6 = open("256.txt", "r")
M7 = open("512.txt", "r")

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

M1_1 = dataread(M1)
M2_1 = dataread(M2)
M3_1 = dataread(M3)
M4_1 = dataread(M4)
M5_1 = dataread(M5)
M6_1 = dataread(M6)
M7_1 = dataread(M7)


M1_2 = mean(M1_1)
M2_2 = mean(M2_1)
M3_2 = mean(M3_1)
M4_2 = mean(M4_1)
M5_2 = mean(M5_1)
M6_2 = mean(M6_1)
M7_2 = mean(M7_1)

print(M1_2)
print(M2_2)
print(M3_2)
print(M4_2)
print(M5_2)
print(M6_2)
print(M7_2)


# MA_MEDIAN = median(MA_VALUE)
# MM_MEDIAN = median(MM_VALUE)
# HY_MEDIAN = median(HY_VALUE)

# print(MA_MEAN)
# print(MM_MEAN)
# print(HY_MEAN)

# print(MA_MEDIAN)
# print(MM_MEDIAN)
# print(HY_MEDIAN)

fig, ax = plt.subplots()

ax.boxplot((M1_1,M2_1,M3_1,M4_1,M5_1,M6_1,M7_1), vert=True, showmeans=True, meanline=True,
           labels=('8','16','32','64','128','256','512'), patch_artist=True,
           medianprops={'linewidth': 2, 'color': 'purple'},
           meanprops={'linewidth': 2, 'color': 'red'})

plt.ylim(ymin=0.0)
plt.legend()

plt.title("Different switch point time cost from 4kb to 2GB")
plt.ylabel("time (ms)")
plt.xlabel("Switch point (KB)")

figure_name = "switch.png"
plt.savefig(figure_name, format='png', bbox_inches='tight')

plt.show()