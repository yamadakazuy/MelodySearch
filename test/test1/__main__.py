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
#        "SinNFA": "../../Privatespace/SinNFA/Debug/SinNFA.exe"
        }

for prog, exe in programs.items():
    for pat in patterns: 
        print(prog, pat)   
        for i in range(5):
            #cmd = ["C:/Users/ayane/Documents/MelodySearch/MyNFA2/Debug/MyNFA2.exe", folder , pat]
            #cmd = ["C:/Users/ayane/Documents/MelodySearch/MyFirst2/Debug/MyFirst2.exe", folder , pat]
            cmd = [exe,folder, pat]
            result = subprocess.run(cmd, stdout=subprocess.PIPE , stderr=subprocess.PIPE ,encoding="utf-8")
    
            # print(result.stdout.split('\n'))
            if i == 0:
                print(result.stdout.split('\n')[-3].split(' ')[2])
            print(result.stdout.split('\n')[-2].split(' ')[2], end='\t')
        print()

# print(sys.argv)
# cmd =  ["C:/Users/ayane/Documents/MelodySearch/MyFirst2/Debug/MyFirst2.exe", folder , pattern]
# result = subprocess.run(cmd, stdout=subprocess.PIPE , stderr=subprocess.PIPE ,encoding="utf-8")
#
# print(result.stdout)