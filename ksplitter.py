import numpy as np
import random as rp
import sys
import os
import string
import secrets
import pickle
import ntpath
import _thread

threadpool = [0]
threads = [4]
working = ['']

def s0thread(a, i,sz):
    f = open(working[0], mode="rb")
    f.seek(a)
    data = f.read(sz)
    # Printing our byte sequenced data 
    F = data
    O = open(working[0] + '.' + str(i), 'wb') 
    O.write(F)
    O.close()
    f.close()
    threadpool[0] += 1
    threads[0] += 1

def path_leaf(path):
    head, tail = ntpath.split(path)
    return tail or ntpath.basename(head)

def splitFile(divisions): #-1
    
    sz = int(os.path.getsize(working[0]) / divisions)

    # Reading file data with read() method

    for i in range(divisions):
        a = (i)*sz
        if i == divisions - 1:
            sz = -1

        _thread.start_new_thread(s0thread,(a,i, sz))
        threads[0] -= 1
        while (threads[0] <= 0):
            v = None

    while (threadpool[0] < divisions):
        v = None
    
    os.remove(working[0])


def mergeFile(divisions): #-1
    f = open(working[0], mode="wb")

    # Reading file data with read() method
    for i in range(divisions):
        sz = int(os.path.getsize(working[0] + '.' + str(i)))
        O = open(working[0] + '.' + str(i), 'rb') 
        data = O.read()
        f.write(data)
        v = f.seek((i+1)*sz)

        O.close()
        os.remove(working[0] + '.' + str(i))
        # Closing the opened file
    f.close()

def mergeFileP(divisions):
    f = open(working[0], mode="wb")
    d = []
    # Reading file data with read() method
    for i in range(divisions):
        sz = int(os.path.getsize(working[0] + '.' + str(i)))
        O = open(working[0] + '.' + str(i), 'rb') 
        data = O.read()
        d.append(data)

        O.close()
        os.remove(working[0] + '.' + str(i))
        # Closing the opened file
    pickle.dump(d, f)
    f.close()

def splitFileP(divisions):
    f = open(working[0], mode="rb")
    d = pickle.load(f)
    # Reading file data with read() method
    i = 0
    for data in d:
        O = open(working[0] + '.' + str(i), 'wb') 
        O.write(data)
        O.close()
        i += 1
        # Closing the opened file
    f.close()

    os.remove(working[0])

def mergeFileO(divisions):
    f = open(working[0], mode="wb")
    szo = 0
    # Reading file data with read() method
    for i in range(divisions):
        sz = int(os.path.getsize(working[0] + '.' + str(i)))
        f.write(sz.to_bytes(8, 'little'))
        szo += 8
        f.seek(szo)

        O = open(working[0] + '.' + str(i), 'rb') 
        data = O.read()
        szo += f.write(data)
        v = f.seek(szo)

        O.close()
     #   print(sz,i,szo)
        os.remove(working[0] + '.' + str(i))
        # Closing the opened file
    
    f.close()

def s1thread(a, i,sz):
    print(i,a,sz)
    f = open(working[0], mode="rb")
    f.seek(a)
    data = f.read(sz)
    # Printing our byte sequenced data 
    F = data
    O = open(working[0] + '.' + str(i), 'wb') 
    O.write(F)
    O.close()
    f.close()
    threadpool[0] += 1
    threads[0] += 1

def splitFileO(divisions):
    f = open(working[0], mode="rb")

    pos = 0
    sz = 1
    i = 0
    # Reading file data with read() method
    while (i < divisions):
    #    print('svt: ', pos,i, sz)
        sz = int.from_bytes(f.read(8), 'little');
        pos += 8
        f.seek(pos)
        data = f.read(sz)
        # Printing our byte sequenced data 
        F = data
        O = open(working[0] + '.' + str(i), 'wb') 
        O.write(F)
        O.close()
    #    _thread.start_new_thread(s1thread,(pos,i, sz))
   #     threads[0] -= 1
        pos += sz

       
        i += 1
    #    while (threads[0] <= 0):
      #      v = None

  # while (threadpool[0] < divisions):
  #      v = None

    f.close()
    
    os.remove(working[0])
