'''
Created on 2021/12/23

@author: sin
'''
import cmath

def cord(t):
    return cmath.exp(cmath.pi*2j*ord(t)/128)

def dft(f, n = 1):
    t = list()
    for y in range(len(f)) :
        t.append(sum([ f[x]*cmath.exp(-2j*cmath.pi/len(f)*y*x) for x in range(len(f))])/n)
    return t

def norm(f):
    nv = list()
    for i in range(len(f)):
        nv.append(abs(f[i]))
    return nv
    
if __name__ == '__main__':
    
    t = [cord(c) for c in 'abbabaababaabbab']
    p = [cord('aabb'[i]) if i < len('aabb') else 0 for i in range(len(t))]
    print(t)
    print(p)
    print(dft(t))
    print(dft(p))
    T = dft(t)
    P = dft(p)
    for i in range(len(T)):
        T[i] = T[i]*P[i]
    print(T)
    r = dft(T,len(T)**2)
    print(norm(r))
    