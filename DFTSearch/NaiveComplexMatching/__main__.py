'''
Created on 2021/12/21

@author: sin
'''
import sys
import math, cmath

def cord(t):
    return cmath.exp(cmath.pi*2j*ord(t)/128)

if __name__ == '__main__':
    textstr = sys.argv[1]
    pattstr = sys.argv[2]
    if len(textstr) < len(pattstr) :
        pattstr, textstr = textstr, pattstr

    text = [ cord(textstr[i]) for i in range(len(textstr))]
    patt = [ cord(pattstr[i]) for i in range(len(pattstr))]
    
    for pos in range(len(text) - len(patt) + 1):
        prod = 0
        for i in range(len(patt)):
            prod += text[pos+i].real * patt[i].real + text[pos+i].imag * patt[i].imag
        print(prod/len(patt))
            