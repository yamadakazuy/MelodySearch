'''
Created on 2022/03/05

@author: Sin Shimozono
'''

import os,sys,datetime
import re

if __name__ == '__main__':
    pattern = re.compile(sys.argv[1])
    mct_dir = '.\oldcollection'
    if len(sys.argv) > 2 :
        mct_dir = sys.argv[2]
    print(pattern, mct_dir)
    
    filecount = 0
    totallength = 0
    start = datetime.datetime.now()
    for fname in os.listdir(mct_dir):
        fullpath = os.path.join(mct_dir, fname)
        if not (os.path.isfile(fullpath) and fullpath.endswith('.mct')) :
            continue
        with open(fullpath, "r") as f:
            text = f.read()
        filecount += 1
        totallength += len(text)
        if pattern.search(text) != None:
            print(fullpath)
    print(datetime.datetime.now() - start)
    print('finished.')
    print('files count ', filecount)
    print('total length ', totallength)