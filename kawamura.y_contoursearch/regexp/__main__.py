'''
Created on 2022/03/05

@author: Sin Shimozono
'''

import os,sys,datetime
import re

if __name__ == '__main__':
    pattern = sys.argv[1]
    if pattern[0] == '*' : pattern = pattern[1:]
    pstr = ''
    for c in pattern:
        if c == '+' :
            pstr += r'[#\+]'
        elif c == '-':
            pstr += r'[b-]'
        else:
            pstr += c
    rex = re.compile(pstr)
    mct_dir = '.\oldcollection'
    print(len(sys.argv))
    if len(sys.argv) > 2 :
        mct_dir = sys.argv[2]
    print(rex, mct_dir)
    
    filecount = 0
    totallength = 0
    start = datetime.datetime.now()
    for fname in os.listdir(mct_dir):
        fullpath = os.path.join(mct_dir, fname)
        if not (os.path.isfile(fullpath) and fullpath.endswith('.mct')) :
            continue
        with open(fullpath, "r") as f:
            tempo = int(float(f.readline()))
            text = f.read()
        filecount += 1
        totallength += len(text)
        res = rex.search(text)
        if res != None:
            print(fullpath, res.span(), tempo)
    print(datetime.datetime.now() - start)
    print('finished.')
    print('files count ', filecount)
    print('total length ', totallength)