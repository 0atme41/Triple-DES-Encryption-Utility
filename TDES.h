/** 
    @file TDES.h
    @author
    Header for the Triple DES Implementation.
*/

#include "IO.h"
#include "magic.h"

/**
 * @param input the input array of bytes to encrypt
 * @param inputLen the length of the input (in bytes)
 * @param key an array of bits - the encryption key
 * @param keyLen the length of the key in bytes
 * @param n the length of the resulting array using pass-by reference
 * @return a pointer to a dynamically allocated array containing the padded, encrypted result
 * performs the TDES encryption process
 */
byte *encryptTDES(byte input[], int inputLen, byte key[], int keyLen, int *n);

/**
 * @param input the input array of bytes to decrypt
 * @param inputLen the length of the input (in bytes)
 * @param key an array of bits - the decryption key
 * @param keyLen the length of the key in bytes
 * @param n the length of the resulting array using pass-by reference
 * @param a a pointer to a dynamically allocated array containing the decrypted result
 * performs the TDES decryption process
 */
byte *decryptTDES(byte input[], int inputLen, byte key[], int keyLen, int *n);
