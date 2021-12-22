'''
Created on 2021/12/21

@author: sin
'''
import sys

if __name__ == '__main__':
    text = sys.argv[1]
    pattern = sys.argv[2]
    if len(text) < len(pattern) :
        pattern, text = text, pattern
    
    pnorm = sum([ord(pattern[i])**2 for i in range(len(pattern))])
    for pos in range(len(text)-len(pattern)+1) :
        innerprod = 0
        for i in range(len(pattern)):
            innerprod += ord(text[pos+i])*ord(pattern[i])
        print(innerprod/pnorm)
