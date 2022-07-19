import numpy as np
import scipy.stats as st
import matplotlib.pyplot as plt
from collections import Counter
import pandas as pd
from statistics import mean,variance,median

pt1 = [18,17,13,17,16,16,15,13,16,16,13,18,20,17,11,17,41,19,17,22,16,12,14,16,13,14,11,42,12,15]
tc1 = [78,86,90,116,118,107,117,119,101,79,122,91,129,76,76,69,134,101,107,127,123,80,171,107,109,110,122,90,78,123]
je1 = [63,56,40,38,44,42,43,64,58,38,51,46,36,58,43,55,48,59,59,36,44,36,47,64,41,54,46,41,70,52]
mi1 = [101,96,74,90,81,77,80,80,75,89,89,92,93,85,82,71,87,85,104,72,97,83,91,92,88,86,104,89,85,85]
me1 = [69,50,55,60,61,61,67,54,71,53,69,71,72,47,65,73,49,65,46,63,82,63,52,78,61,74,50,59,67,61]

pt2 = [4589,4829,4816,4757,4821,4771,4716,4759,4779,4819,4556,4793,4856,6174,4829,4766,4733,7161,4869,4779,4757,4843,4776,4868,4778,4741,4828,4913,4788,5206]
tc2 = [538631,504130,505599,503297,504728,506257,501266,508429,500223,503630,502032,501376,505557,503573,509086,510664,499369,505709,504027,511412,517970,504685,503113,528556,507797,501119,502916,504834,502874,506237]
je2 = [491900,490399,496648,495673,503023,512595,497248,496746,490119,493988,503496,496661,496004,514022,490949,484786,507126,495898,505782,499065,483963,490477,506070,494866,493603,493374,481551,505971,498325,491128]
mi2 = [558035,555719,556934,555632,558358,560106,559788,564233,557709,557788,564903,564327,577214,562675,568554,570176,567886,562461,568933,565188,572299,567190,567793,563524,565746,560594,566456,558253,569756,553796]
me2 = [475,487,529,512,535,457,529,546,544,503,476,474,509,481,463,504,497,617,507,511,574,498,568,602,525,487,586,486,466,547]

fig, axs = plt.subplots(2)

axs[0].boxplot((pt1,tc1,je1,mi1,me1), vert=True, showmeans=True, meanline=True,
           labels=('Ptmalloc2','Tcmalloc','Jemalloc','Mimalloc','Mremap'), patch_artist=True,
           medianprops={'linewidth': 0, 'color': 'purple'},
           meanprops={'linewidth': 2, 'color': 'red'})

# plt.legend()

axs[0].set_title("a. 4 KiB to 128 KiB")
axs[0].set(ylabel = 'time (us)')
axs[0].label_outer()
axs[0].yaxis.get_label().set_fontsize(16)
# plt.xlabel("Realloc method (MA,MM,HY)")

axs[1].boxplot((pt2,tc2,je2,mi2,me2), vert=True, showmeans=True, meanline=True,
           labels=('Ptmalloc2','Tcmalloc','Jemalloc','Mimalloc','Mremap'), patch_artist=True,
           medianprops={'linewidth': 0, 'color': 'purple'},
           meanprops={'linewidth': 2, 'color': 'red'})

# plt.legend()

axs[1].set_title("b. 4 KiB to 2 GiB")
axs[1].set(yscale="log")
axs[1].yaxis.get_label().set_fontsize(14)
plt.ylabel("time (us)")
# plt.xlabel("allocators")


figure_name = "microbenchmark.png"
plt.savefig(figure_name, format='png', bbox_inches='tight')

plt.show()