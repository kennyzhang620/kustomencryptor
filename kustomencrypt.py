from kencrypt import *

def help():
    print("ENCRYPTION")
    print("Encrypt single file with random password")
    print('kustomencrypt <source> --e [key-size] [min-matrix-cond]')
    print("Encrypt single file with user password")
    print('kustomencrypt <source> --ep <password> [key-size] [min-matrix-cond]')
    print("Encrypt single file with random password given key")
    print('kustomencrypt <source> <key> --ek')
    print("Encrypt single file with user password given key")
    print('kustomencrypt <source> <key> --ekp <password>')
    print("DECRYPTION")
    print("Decrypt scrambled file")
    print('kustomencrypt <ciphertext> <key> --d <password>')
    print("ADVANCED")
    print('Get source matrix size')
    print('kustomencrypt <source> --s')
    print('Generate encryption key with minimum matrix size and condition')
    print('kustomencrypt <name> --k <min-size> [min-matrix-cond]')
    print('Generate secure hash given base, exponent given key')
    print('kustomencrypt <base> <key> --sh <exponent>')

def main():
    try:
        if (len(sys.argv) == 1):
            testEncryption()

        if (len(sys.argv) == 2 and sys.argv[1] == '--help'):
            VALUES[0] = 0
            help()

        if (len(sys.argv) == 3):
            if (sys.argv[2] == '--e'):
                encryptFile()
                print("Encryption password:", VALUES[3])
            if (sys.argv[2] == '--s'):
                getSize()    
            if (sys.argv[2] == '--k'):
                generateKey()
                VALUES[0] = 0

        if (len(sys.argv) == 4):
            if (sys.argv[2] == '--e'):
                encryptFile(int(sys.argv[3]))
                print("Encryption password:", VALUES[3])

            if (sys.argv[2] == '--ep'):
                VALUES[3] = sys.argv[3]
                encryptFile()

            if (sys.argv[2] == '--k'):
                generateKey(sys.argv[3])
                VALUES[0] = 0
            
            if (sys.argv[3] == '--ek'):
                encryptKey()
                print("Encryption password:", VALUES[3])

        if (len(sys.argv) == 5):
            if (sys.argv[2] == '--ep'):
                VALUES[3] = sys.argv[3]
                encryptFile(int(sys.argv[4]))
            if (sys.argv[2] == '--k'):
                generateKey(sys.argv[3], sys.argv[4])
                VALUES[0] = 0
            if (sys.argv[3] == '--ekp'):
                VALUES[3] = sys.argv[4]
                encryptKey()
            if (sys.argv[3] == '--sh'):
                VALUES[3] = sys.argv[4]
                s = sys.argv[1]
                b = bytearray()
                b.extend(map(ord, s))
                print(''.join(list(map(chr, list(map(lambda x: x % 127 + 33,list(generateHashedBytes(b)))))))  )
            if (sys.argv[3] == '--d'):
                decryptFile()
                VALUES[0] = 0
        
        if (len(sys.argv) == 5):
            if (sys.argv[2] == '--e'):
                encryptFile(int(sys.argv[3]), int(sys.argv[4]))
                print("Encryption password:", VALUES[3])     
            
        if (len(sys.argv) == 6):
            if (sys.argv[2] == '--ep'):
                VALUES[3] = sys.argv[3]
                encryptFile(int(sys.argv[4]), int(sys.argv[5]))


        if (VALUES[0] != None):     
            print("Operation complete!")
        else:
            print("Incorrect operation specified. Showing help menu.")
            help()
    except Exception as e:
        print("Operation failed:", e)

if __name__ == '__main__':
    main()
    
