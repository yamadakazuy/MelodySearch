'''
Created on 2021/12/23

@author: sin
'''
# 1. テキスト、パターンを単位複素数の列に変換。
#    パターンは逆順でかつ複素共役にしておく
# 2. それぞれフーリエ変換
# 3. 変換された列の要素ごとの積をとる
# 4. 積の列をフーリエ逆変換
# 5. 要素が実部のみで == パターン長となっているところが出現位置-1

from cmath import exp, pi
from math import ceil, log2
import numpy
import sys, re

def cxarray(alphabet : dict, text : str, n = None, conjugate = None, reverse = None):
    #assumes s is a str or byte seq.
    if n == None :
        n = len(text)
    # 長さが n 以上の最小の 2 のべきとなる単位ベクトルの列に変換
    # FFT では必要
    n = 1 << ceil(log2(max(len(text),n)))
    res = numpy.array([0] * n, dtype=numpy.complex64)
    coeff = pi * 2j
    if conjugate != None:
        coeff = -coeff
    for i in range(len(text)) :
        if reverse == None :
            res[i] = exp(coeff * alphabet[text[i]]/len(alphabet))
        else:
            res[-i-1] = exp(coeff * alphabet[text[i]]/len(alphabet))
    return res
    
def cxstr(a):
    return str(['({0.real:.3f} {0.imag:+.3f}i)'.format(e) for e in a])
    
def dft(f):
    t = f.copy()
    for y in range(len(t)) :
        tot = 0
        for x in range(len(f)):
            tot += f[x] * exp(-2j * pi / len(f) * y * x)
        t[y] = tot
    return t

def idft(f):
    t = f.copy()
    for y in range(len(t)) :
        tot = 0
        for x in range(len(f)):
            tot += f[x].conjugate() * exp(-2j * pi / len(f) * y * x)
        t[y] = tot/len(t)
    return t
    
if __name__ == '__main__':
    alph = { 'a': 0, 'b': 1 }
    text = sys.argv[1]
    patt = sys.argv[2]
    n = max(len(text), len(patt))
    print("{}\n{}\n{}\n".format(text, patt, n) )
    # そのまま複素アルファベットのベクトルに変換
    textvec = cxarray(alph, text, n, conjugate = -1)
#    print('vector length = ',n)
    # 前後が逆で共役なベクトルに変換
    pattvec = cxarray(alph, patt, n, reverse = -1)
    print(cxstr(textvec))
    print(cxstr(pattvec))
    print()
    
    dfttext = dft(textvec)
    dftpatt = dft(pattvec)
#    print(cxstr(dfttext))
#    print(cxstr(dftpatt))
#    print()

    # 二つの列の要素毎の積を持つ列を求める    
    dfttext *= dftpatt

    # 積の列を逆変換する
    textvec = idft(dfttext)
    print(cxstr(textvec))
    print()
    
    # 完全一致（絶対値＝パターン長）と判断する誤差の幅
    x = cxarray(alph, 'ab')
    # アスキーコードがとなりあう文字の場合に生じる差を完全一致の閾値につかう
    epsilon = 1 - (x[0].real * x[1].real + x[0].imag * x[1].imag)
    absvec = []
    print('epsilon =',epsilon)
    for i in range(len(textvec)):
        absvec.append(abs(textvec[i]))
        if abs(textvec[i].real - len(patt)) < epsilon :
            # パターンは見つかった位置の次から出現している
            print((i+1) % len(textvec))
    print('occurrences by re.find: ', [m.span()[0] for m in re.finditer(patt,text)])
    print(['{:.2f}'.format(val) for val in absvec])
# python3 DFTStringMatching textstring patternstring
# abadabda abd
