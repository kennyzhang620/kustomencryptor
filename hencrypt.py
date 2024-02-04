import secrets
import sys 
import os
import pickle
import numpy as np

from kencrypt import decryptHeader, unwrap, wrap, VALUES
import _thread

NP_FLOAT_MAX = 8
RANDMAX = 2**31
threadpool = [0]
threads = [4]
def help():
    print("HIGH DENSITY FILE ENCRYPTOR")
    print('hencrypt <source> <key> <--e/--d> <passwd> <block-size>')

def ethread(rv, i):
   # print('python kustomencrypt.py ' + '"' + sys.argv[1] + '.' + str(i) + '"'  + ' ' + '"' + sys.argv[2] + '"' + ' --ekp ' + '"' + sys.argv[4]  + '"  "' + rv + '.' + str(i) + '"')
    os.system('python kustomencrypt.py ' + '"' + sys.argv[1] + '.' + str(i) + '"'  + ' ' + '"' + sys.argv[2] + '"' + ' --ekp ' + '"' + sys.argv[4]  + '"  "' + rv + '.' + str(i) + '"') 
    threadpool[0] += 1
    threads[0] += 1

def dthread(i, j):
    os.system('python kustomencrypt.py ' + '"' + sys.argv[1]  + '.' + str(i) + '" ' + '"' + sys.argv[2] + '"' + ' --d ' + '"' + sys.argv[4] + '"') 
    threadpool[0] += 1
    threads[0] += 1

def encrypt():
    print("Performing HD file encryption with", threads[0], 'threads.')
    # hencrypt <source> <key> <--e/--d> <passwd> <frags>
    frags = (int(sys.argv[5])) if int(sys.argv[5]) >= 2 else 2
    os.system('python filelib.py ' + '"' + sys.argv[1] + '"' + ' --b '  + str(frags)) 
    rv = str(secrets.randbelow(RANDMAX))
  #  print(os.path.join(os.path.dirname(sys.argv[1]), rv))
    for i in range(frags):
        _thread.start_new_thread(ethread, (rv, i))
        threads[0] -= 1
        while (threads[0] <= 0):
            v = None

    while (threadpool[0] < frags):
        v = None

    os.system('python filelib.py "'  + os.path.join(os.path.dirname(sys.argv[1]), rv) + '" --mo '  + str(frags)) 
    print("Encrypted as", rv)
    exit()

def decrypt():
    print("Performing HD file decryption with", threads[0], 'threads.')
    # hencrypt <source> <key> <--e/--d> <passwd> <frags>
    VALUES[3] = sys.argv[4]
    frags = int(int(sys.argv[5])/NP_FLOAT_MAX) if int(sys.argv[5])/NP_FLOAT_MAX >= 2 else 2
    os.system('python filelib.py ' + '"' + sys.argv[1] + '"' + ' --bo '  + str(frags)) 
    fi = open(sys.argv[1] + '.0', 'rb')
    B = np.load(sys.argv[2], allow_pickle=True)
    A = pickle.load(fi)
    fi.close()
    header = decryptHeader(A[0], B).rstrip('\x00')
    header = header[:len(header) - 2]

    for i in range(frags):
        _thread.start_new_thread(dthread,(i,0))
        threads[0] -= 1
        while (threads[0] <= 0):
            v = None

    while (threadpool[0] < frags):
        v = None

    os.system('python filelib.py "' + os.path.join(os.path.dirname(sys.argv[1]), header) + '" --m '  + str(frags)) 
    print("Decrypted as", header)
    exit()


def main():

    if (len(sys.argv) != 6):
        help()
    else:
        mb = 1024*10**3
        fs = os.path.getsize(sys.argv[1])
        div = (int(sys.argv[5]))

        ERR = "Block size must be "+ str(NP_FLOAT_MAX) + " MB or higher!"
        if div < NP_FLOAT_MAX:
            raise RuntimeError(ERR)

        frags = round(fs/((div*mb)/NP_FLOAT_MAX)) if fs/((div*mb)/NP_FLOAT_MAX) > 1 else fs 
       # print(fs,mb, (frags,  round(fs/frags)), round(fs/frags)*frags)
        sys.argv[5] = min(frags,  round(fs/frags))

        if (sys.argv[3] == '--e'):
            encrypt()
        if (sys.argv[3] == '--d'):
            decrypt()


if __name__ == '__main__':
    main()
    
