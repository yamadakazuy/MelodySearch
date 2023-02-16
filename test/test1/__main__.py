'''
Created on 2023/01/27

@author: ayane.h
'''

import subprocess
import sys

# pattern = sys.argv[1]

patterns = sys.argv[1:]

folder = "../../MelodySearch/smf/ayanetest"

programs = {"NFA2": "../../MelodySearch/MyNFA2/Debug/MyNFA2.exe" , 
        "First2": "../../MelodySearch/MyFirst2/Debug/MyFirst2.exe",
        "xxxNFA": "../../Privatespace/SinNFA/Debug/SinNFA.exe"
        }

for prog, exe in programs.items():
    print(prog)
    for pat in patterns: 
        print(pat, end='\t')
        hist = []
        for i in range(10):
            #cmd = ["C:/Users/ayane/Documents/MelodySearch/MyNFA2/Debug/MyNFA2.exe", folder , pat]
            #cmd = ["C:/Users/ayane/Documents/MelodySearch/MyFirst2/Debug/MyFirst2.exe", folder , pat]
            cmd = [exe,folder, pat]
            result = subprocess.run(cmd, stdout=subprocess.PIPE , stderr=subprocess.PIPE ,encoding="utf-8")
    
            # print(result.stdout.split('\n'))
            if i == 0:
                print(result.stdout.split('\n')[-3].split(' ')[2] + '\t', end='')
            hist.append(int(result.stdout.split('\n')[-2].split(' ')[2]))
        hmax = max(hist)
        hmin = min(hist)
        avr = (sum(hist) - hmax - hmin) / (len(hist) - 2)
        print('xavr\t' + str(avr) + '\tmax\t' + str(hmax) + '\tmin\t' + str(hmin))
    print()
# print(sys.argv)
# cmd =  ["C:/Users/ayane/Documents/MelodySearch/MyFirst2/Debug/MyFirst2.exe", folder , pattern]
# result = subprocess.run(cmd, stdout=subprocess.PIPE , stderr=subprocess.PIPE ,encoding="utf-8")
#
# print(result.stdout)