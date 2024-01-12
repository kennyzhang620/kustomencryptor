# kustomencryptor
A custom toy encryption library utilizing Randomised substitution+transpose cipher and Hill cipher.

# HOW TO USE

## ENCRYPTION
Encrypt single file with random password
kustomencrypt <source> --e [key-size] [min-matrix-cond]
Encrypt single file with user password
kustomencrypt <source> --ep <password> [key-size] [min-matrix-cond]
Encrypt single file with random password given key
kustomencrypt <source> <key> --ek
Encrypt single file with user password given key
kustomencrypt <source> <key> --ekp <password>

##DECRYPTION
Decrypt scrambled file
kustomencrypt <ciphertext> <key> --d <password>

##ADVANCED
Get source matrix size
kustomencrypt <source> --s
Generate encryption key with minimum matrix size and condition
kustomencrypt <name> --k <min-size> [min-matrix-cond]
Generate secure hash given base, exponent given key
kustomencrypt <base> <key> --sh <exponent>
