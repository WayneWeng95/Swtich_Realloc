import matplotlib.pyplot as plt
import numpy as np


labels = ['SET', 'GET', 'INCR', 'LPUSH', 'RPUSH']
glibc = [138888,141043,150829,144508,136612]
tcmalloc = [169779,180831,158227,168067,177619]
jemalloc = [166666,154559,177304,171232,161030]
Adaptive = [178890,174520,186567,176678,173611]

x = np.arange(len(labels))  # the label locations
width = 0.2  # the width of the bars

fig, ax = plt.subplots()
rects1 = ax.bar(x - width*1.5, glibc, width, label='Glibc')
rects2 = ax.bar(x - width/2, tcmalloc, width, label='Tcmalloc')
rects3 = ax.bar(x + width/2, jemalloc, width, label='Jemalloc')
rects4 = ax.bar(x + width*1.5, Adaptive, width, label='Adaptive')

# Add some text for labels, title and custom x-axis tick labels, etc.
ax.set_ylabel('Request per seconds',fontsize=16)
ax.set_title('Redis-benchmark with different allocators',fontsize=18)
ax.set_xticks(x, labels)
ax.legend()

# ax.bar_label(rects1, padding=3)
# ax.bar_label(rects2, padding=3)
# ax.bar_label(rects3, padding=3)
# ax.bar_label(rects4, padding=3)

figure_name = "Redis-bench(4).png"
plt.savefig(figure_name, format='png', bbox_inches='tight')

fig.tight_layout()

plt.show()