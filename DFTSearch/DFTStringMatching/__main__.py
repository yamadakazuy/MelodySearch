'''
Created on 2021/12/23

@author: sin
'''
from cmath import exp, pi
import numpy
import sys

def cxarray(s, n = None, conjrev = 1):
    #assumes s is a str or byte seq.
    if n == None :
        n = 0
    #n = 1<<ceil(log2(max(len(s),n)))
    n = max(len(s),n)
    res = numpy.array([0] * n, dtype=numpy.complex64)
    coeff = pi * 2j
    for i in range(len(s)) :
        if conjrev >= 0 :
            res[i] = exp(coeff * ord(s[i])/128.00)
        else:
            res[-i-1] = exp(-coeff * ord(s[i])/128.00)
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
        t[y] = tot.conjugate()/len(t)
    return t
    
if __name__ == '__main__':
    text = sys.argv[1]
    patt = sys.argv[2]
    n = max(len(text), len(patt))
    print(text, patt, n)
    # 長さが n 以上の最小の 2 のべきとなる単位ベクトルの列に変換
    textvec = cxarray(text,n)
    print('vector length = ',n)
    # パターンは前後が逆順でベクトルは共役なものに変換
    pattvec = cxarray(patt,n,-1)
    print(cxstr(textvec))
    print(cxstr(pattvec))
    print()
    
    dfttext = dft(textvec)
    dftpatt = dft(pattvec)
    print(cxstr(dfttext))
    print(cxstr(dftpatt))
    print()

    # 二つの列の要素毎の積を持つ列を求める    
    dfttext *= dftpatt

    # 積の列を逆変換する
    textvec = idft(dfttext)
    print(cxstr(textvec))
    print()
    
    # 完全一致（絶対値＝パターン長）と判断する誤差の幅
    epsilon = 1/1024.0
    for i in range(len(textvec)):
        if abs(textvec[i].real - len(patt)) < epsilon :
            # パターンは見つかった位置の次から出現している
            print((i+1) % len(textvec))
    