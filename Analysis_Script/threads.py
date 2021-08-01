import numpy as np
import scipy.stats as st
import matplotlib.pyplot as plt
from collections import Counter
import pandas as pd
from statistics import mean

thread = [1, 2, 3, 4, 5, 6, 7, 8]
j = 0
times_MA = [None]*8
times_MM = [None]*8
files_MA = [None]*8
files_MM = [None]*8
MA_Average = [None]*8
MM_Average = [None]*8


for i in thread:
    times_MA[j] = open(
        "results/parallel/MA/K/512/%s.txt" % i, "r")
    times_MM[j] = open(
        "results/parallel/MM/K/512/%s.txt" % i, "r")
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


for j in range(8):
    files_MA[j] = dataread(times_MA[j])
    MA_Average[j] = round(mean(files_MA[j]), 2)
    files_MM[j] = dataread(times_MM[j])
    MM_Average[j] = round(mean(files_MM[j]), 2)

# time1 = dataread(file1)

# print(time1)

# time2 = dataread(times[0])

# print(time2)

print(MA_Average)
print(MM_Average)

plt.plot(MA_Average, marker='o', label='Realloc')
plt.plot(MM_Average, marker='o', label='Mremmap')

# fig, ax = plt.subplots()
# ax.boxplot((files_MA[0], files_MA[1],files_MA[2], files_MA[3],files_MA[4], files_MA[5],files_MA[6], files_MA[7]), vert=True, showmeans=True, meanline=True,
#            labels=('1 thread', '2 threads','3 threads', '4 threads','5 threads', '6 threads','7 threads', '8 threads'), patch_artist=True,
#            medianprops={'linewidth': 2, 'color': 'purple'},
#            meanprops={'linewidth': 2, 'color': 'red'})

# ax.boxplot((files_MM[0], files_MM[1],files_MM[2], files_MM[3],files_MM[4], files_MM[5],files_MM[6], files_MM[7]), vert=True, showmeans=True, meanline=True,
#            labels=('1 thread', '2 threads','3 threads', '4 threads','5 threads', '6 threads','7 threads', '8 threads'), patch_artist=True,
#            medianprops={'linewidth': 2, 'color': 'purple'},
#            meanprops={'linewidth': 2, 'color': 'red'})

plt.ylim(ymin=0.0)
plt.legend()
plt.xticks([0, 1, 2, 3, 4, 5, 6, 7], ['1', '2', '3', '4','5', '6', '7', '8'])

plt.title("multi-thread with 512kB data")
plt.ylabel("time (ms)")
plt.xlabel("threads")

figure_name = "512kb thread.png"
plt.savefig(figure_name, format='png', bbox_inches='tight')

plt.show()
