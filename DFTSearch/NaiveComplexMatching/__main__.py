'''
Created on 2021/12/21

@author: sin
'''
# ナイーブなマッチングを、文字の比較のかわりに
# 文字を単位複素数に変換してそれらを要素に持つベクトルの内積 == 1 かどうかで判定する。。

import sys
from cmath import exp, pi

def cxord(t):
    return exp(pi*2j*ord(t)/128)

if __name__ == '__main__':
    textstr = sys.argv[1]
    pattstr = sys.argv[2]
    if len(textstr) < len(pattstr) :
        pattstr, textstr = textstr, pattstr
    print(pattstr, textstr)
    text = [ cxord(textstr[i]) for i in range(len(textstr))]
    patt = [ cxord(pattstr[i]) for i in range(len(pattstr))]
    
    for pos in range(len(text) - len(patt) + 1):
        prod = 0
        for i in range(len(patt)):
            prod += text[pos+i].real * patt[i].real +  text[pos+i].imag * patt[i].imag
        print(prod/len(patt))
            