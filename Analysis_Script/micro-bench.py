import numpy as np
import scipy.stats as st
import matplotlib.pyplot as plt
from collections import Counter
import pandas as pd
from statistics import mean,variance

sizes = [4,8,16,32,64,128,256,512,1024] #Mb level
threads = [1,2,4,8] 

DATA_LENGTH = len(sizes)

MA = [4530,mean([5578,5237]),mean([6465,5263,7268,9360]),mean([6291,10429,5754,4479,21139,9862,6033,2720])]
MM = [488,mean([996,494]),mean([1676,1511,1486,2579]),mean([2419,2345,3551,2877,1889,4050,2532,3063])]
HY = [597,mean([692,1909]),mean([1290,2784,2351,3305]),mean([2800,4172,3934,3983,2908,3459,4159,5251])]
TC = [560826,mean([739909,742319]),mean([1019027,1007269,1022220,994676]),mean([988405,1434630,1596456,1606151,1645469,1667238,1661979,1710461])]
JE = [76973,mean([644974,751340]),mean([463606,1055276,1029563,1044032]),mean([677883,1753182,1755902,1754136,1602048,1729694,1714166,1737483])]
MI = [574820,mean([754869,808951]),mean([999369,1054029,1102028,1183616]),mean([1741381,1621591,1788589,1915245,2027235,2056320,2089618,2228438])]

for i in range(len(threads)):
    MA[i]/=1000
    MM[i]/=1000
    HY[i]/=1000
    TC[i]/=1000
    JE[i]/=1000
    MI[i]/=1000


MM1 = [193,190,188,278,305,200,333,401,480]
MA1 = [164,171,242,360,461,676,856,1391,2386]
HY1 = [190,324,355,305,548,602,464,550,586]
TC1 = [2386,4299,6228,10298,18221,32979,64572,126742,277928]
JE1 = [1502,3250,3068,3436,4096,3566,3136,81312,75855]
MI1 = [1713,4110,5967,10154,18308,34240,69343,139610,281112]

fig, axs = plt.subplots(2,sharex=False)

axs[0].plot(MA1, marker='.', label='Glibc',color = 'blue')#,yerr = MA_variance)
axs[0].plot(MM1, marker='.', label='Mremmap',color = 'purple')#,yerr = MM_variance)
axs[0].plot(HY1, marker='o', label='Adaptive',color = 'green')#,yerr = HY_variance)
axs[0].plot(TC1, marker='x', label='TCmalloc',color = 'red')#,yerr = MA_variance)
axs[0].plot(JE1, marker='x', label='Jemalloc',color = 'orange')#,yerr = MM_variance)
axs[0].plot(MI1, marker='x', label='Mimalloc',color = 'brown')#,yerr = HY_variance)

axs[0].legend(fontsize=10)
# axs[0].set_title("a.",y=-0.01)
axs[0].set(xlabel = 'reallocation size (MB)',ylabel = 'reallocation time (us)',yscale='log')
axs[1].set_xticks([0,1,2,3,4,5,6,7,8])
axs[0].set_xticklabels(["0","4","8","16","32","64","128","256","512","1024"])
axs[1].tick_params(labelsize=8)
axs[0].xaxis.get_label().set_fontsize(14)
axs[0].yaxis.get_label().set_fontsize(14)


axs[1].plot(MA, marker='.', label='Glibc',color = 'blue')#,yerr = MA_variance)
axs[1].plot(MM, marker='.', label='Mremmap',color = 'purple')#,yerr = MM_variance)
axs[1].plot(HY, marker='o', label='Adaptive',color = 'green')#,yerr = HY_variance)
axs[1].plot(TC, marker='x', label='TCmalloc',color = 'red')#,yerr = MA_variance)
axs[1].plot(JE, marker='x', label='Jemalloc',color = 'orange')#,yerr = MM_variance)
axs[1].plot(MI, marker='x', label='Mimalloc',color = 'brown')#,yerr = HY_variance)

# axs[1].set_title("b.",y=-0.01)
axs[1].set(xlabel = 'threads',ylabel = 'reallocation time (ms)',yscale='log')
axs[1].set_xticks([0,1,2,3])
axs[1].set_xticklabels(["1","2","4","8"])
axs[1].tick_params(labelsize=14)
axs[1].xaxis.get_label().set_fontsize(14)
axs[1].yaxis.get_label().set_fontsize(14)


# plt.rcParams.update({'font.size': 20})

figure_name = "microbenchresult.png"
plt.savefig(figure_name, format='png', bbox_inches='tight')

plt.show()
