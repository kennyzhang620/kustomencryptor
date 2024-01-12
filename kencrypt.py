import numpy as np
import random as rp
import sys
import os
import string
import secrets

def generateRandStr():
    letters = list(string.ascii_lowercase + string.ascii_uppercase + string.punctuation + string.digits)
    return ''.join(secrets.choice(letters) for i in range(10))

message = ""
VALUES = [None,None, 2**256, generateRandStr()]

def IO_String_Hash(strI, max_range):
    hash = 0;
    for i in strI:
        chr = ord(i)
        hash = (hash << 5) - hash + chr;
        hash |= 0; # Convert to 32bit integer
        
    return hash % max_range

def scatter(seed: str):
    r = IO_String_Hash(seed, VALUES[2])
    cipher = np.linspace(0, VALUES[1], num=VALUES[1]+1)
    rp.seed(r)
    rp.shuffle(cipher)
    return cipher

def unscatter(seed: str):
    cipher = scatter(seed)
    out = dict()

    for i in range(cipher.shape[0]):
        out[int(cipher[i])] = i
    
    return out

def unscramble(x, cipher):
    return int(cipher[x])
    #res = np.where(cipher==x)[0]
    #return int(res)

def scramble(x, cipher):
    return int(cipher[x])

def unwrap(st, pad=-1):
    uw = []
    for a in st:
        uw.append(a)

    VALUES[0] = min(uw)
    VALUES[1] = max(uw)

    sz = len(uw)
    ns = int(np.sqrt(sz) + 1)
    r = int(ns**2 - sz)

    
    if (pad != -1):
        ns = pad
        inc = 0;
        if (sz % ns != 0):
            inc = 1

        r = int(np.floor(sz/ns)) + inc
        r = int(ns*r - len(uw)) 

    for i in range(r):
        uw.append(0)

    print("Wrapping...")

    CAESAR = scatter(VALUES[3])     
    uw = list(map(lambda a: scramble(a, CAESAR), uw))
    uw = list(map(lambda a: scramble(a, CAESAR), uw))
    uw = list(map(lambda a: scramble(a, CAESAR), uw))

    if pad == -1:
        return np.array(uw).reshape(ns,ns)
    else:
        return np.array(uw).reshape(ns,-1)


def encrypt(uw, key, unroll):
    if unroll:
        uw = uw.reshape(key.shape[0],-1)

    print("Encrypting...")
    return np.matmul(key, uw)

def decrypt(cipher, key_inv, unroll):
    if unroll:
        cipher = cipher.reshape(key_inv.shape[0],-1)

    print("Decrypting...")
    return np.matmul(key_inv, cipher)

def clamp(x):
    if (x < 0 or x >= 1000):
        return 0

    return x
    
def wrap(mat):
    A = mat.reshape(-1,1)
    res = []

    CAESAR = unscatter(VALUES[3])

    print("Unwrapping...")
    for v in A:
        res.append(clamp(unscramble(unscramble(unscramble(round(v[0]), CAESAR),CAESAR),CAESAR)))

    return bytes(res)

def key_gen(s, d=-1, condMin = -1):

    if d<= 4:
        raise Exception("Supplied matrix value below 4")

    if d != -1:
        s = d

    R = []
    for x in range(s*s):
        R.append(secrets.choice(range(-100000,100000)))

    R = np.array(R).reshape((s,s)).astype(int)
    print("Generating", R.nbytes*8, "bit key, minimum condition:", condMin);

    while (np.linalg.cond(R) < condMin):
        R = []
        for x in range(s*s):
            R.append(secrets.choice(range(-100000,100000)))
        R = np.array(R).reshape((s,s)).astype(int)

    print("Key generated!")

    return R


def key_inv(mat):
    return np.linalg.inv(mat)

def getSize():
    f = open(sys.argv[1], mode="rb")
     
    # Reading file data with read() method
    data = f.read()
    # Printing our byte sequenced data 
    F = data
    
    U = unwrap(F)
    print(U.shape)

def encryptFile(d=8, cond=500000): #-1
    f = open(sys.argv[1], mode="rb")
     
    # Reading file data with read() method
    data = f.read()
    # Printing our byte sequenced data 
    F = data
     
    # Closing the opened file
    f.close()
    K = key_gen(0, d=d,condMin=cond)

    U = unwrap(F, K.shape[0])
    K_inv = key_inv(K)

    cipher = encrypt(U, K, unroll=True)

    np.save(sys.argv[1] + '.cipher.npy', cipher)
    np.save(sys.argv[1] + '.key.npy', K_inv)

def decryptFile():
    VALUES[3] = sys.argv[4]

    A = np.load(sys.argv[1], allow_pickle=True)
    B = np.load(sys.argv[2], allow_pickle=True)

    testd = decrypt(A, B, unroll=True)
    VALUES[1] = np.max(testd).astype(int)
    R = wrap(testd)

    f = open(sys.argv[1] + '.plain.txt',mode="wb")
    f.write(bytes(R))
    f.close()

def generateKey(d=8, cond=500000): #-1
    K = key_gen(0, d=int(d),condMin=int(cond))
    K_inv = key_inv(K)
    np.save(sys.argv[1] + '.key.npy', K_inv)

def encryptKey():
    f = open(sys.argv[1], mode="rb")
    B = np.load(sys.argv[2], allow_pickle=True)
     
    # Reading file data with read() method
    data = f.read()
    # Printing our byte sequenced data 
    F = data
             
    # Closing the opened file
    f.close()

    U = unwrap(F, B.shape[0])
    cipher = encrypt(U, np.linalg.inv(B), unroll=True)

    np.save(sys.argv[1] + '.cipher.npy', cipher)


def generateHashedBytes(bytein: bytes):
    B = np.load(sys.argv[2], allow_pickle=True)
     
    # Reading file data with read() method
    # Printing our byte sequenced data 
    F = bytein

    rp.seed(VALUES[3])

    B[rp.choice(range(B.shape[0])), :] = 0
    B[rp.choice(range(B.shape[0])), :] = 0

    U = unwrap(F, B.shape[0])
    cipher = encrypt(U, B, unroll=True)

    return bytes(cipher.reshape(-1,1))


def testEncryption():
    msg = '''
ShallowCraft SMP Season 3 is online now

Java version 1.20.1
Bedrock version (idk, but you can also join with bedrock)

Minecraft server details:
Server IP: 51.79.37.5:25600
Dynamic Online Map at: http://51.79.37.5:8179/#world:0:0:0:1500:0:0:0:0:perspective

Main plugins:
bluemap (online map)
gringotts (emerald based currency)
levelled mobs
quickshop (chest shops)
'''
    print("Testing encryption")
        
    s = "ABCDDEFG"
    b = bytearray()
    b.extend(map(ord, s))
    print(wrap(unwrap(b)).decode("utf-8") )

    b = bytearray()
    b.extend(map(ord, msg))
    
    K = key_gen(0, d=10,condMin=500000)
    U = unwrap(b, K.shape[0])

    K_inv = key_inv(K)

    print("E(x)", K)
    print("D(x)", K_inv)
    print("Cond:", np.linalg.cond(K))
    cipher = encrypt(U, K, unroll=True)

    print(msg)
    #print(U)
    #print(cipher)
    #print(cipher.shape)
    #print(wrap(cipher))

    testd = decrypt(cipher, K_inv, unroll=True)
    print(wrap(testd).decode("utf-8") )

    assert msg in wrap(testd).decode("utf-8")