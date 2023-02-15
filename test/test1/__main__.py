'''
Created on 2023/01/27

@author: ayane.h
'''

import subprocess
import sys

# pattern = sys.argv[1]

patterns = sys.argv[1:]

folder = "C:/Users/ayane/Documents/MelodySearch/smf/ayanetest"

cmds = {"NFA2": "C:/Users/ayane/Documents/MelodySearch/MyNFA2/Debug/MyNFA2.exe" , 
        "First2": "C:/Users/ayane/Documents/MelodySearch/MyFirst2/Debug/MyFirst2.exe"
        }

for prog in ["NFA2", "First2"]:
    for pat in patterns: 
        print(prog, pat)   
        for i in range(5):
            #cmd = ["C:/Users/ayane/Documents/MelodySearch/MyNFA2/Debug/MyNFA2.exe", folder , pat]
            #cmd = ["C:/Users/ayane/Documents/MelodySearch/MyFirst2/Debug/MyFirst2.exe", folder , pat]
            cmd = [cmds[prog],folder, pat]
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