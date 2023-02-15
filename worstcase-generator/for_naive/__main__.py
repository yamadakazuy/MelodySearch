'''
Created on 2023/02/02

@author: sin
'''

import sys

if __name__ == '__main__':
    if (len(sys.argv) == 1) :
        print("usage: command [length] [output file name]")
        exit(0)
    
    if (len(sys.argv) <= 1) :
        outlength = 1024
    else:
        outlength = max(1024,int(sys.argv[1]))
    if (len(sys.argv) <= 2) :
        outfilename = "output.cont"
    else:
        outfilename = sys.argv[2]
    
    print("output file length = " + str(outlength))
    
    with open(outfilename, "w") as of :
        of.write("0")
        for i in range(outlength - 1) :
            of.write("=====+")
    
    