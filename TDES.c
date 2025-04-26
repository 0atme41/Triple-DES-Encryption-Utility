/** 
    @file TDES.c
    @author
    Implementation of the DES / Triple DES algorithm.
*/

#include "TDES.h"
#include "magic.h"
#include "TDESinternal.h"

/** the amount to decrement the index for 0-based indexing */
#define INDEX_DECREMENT 1
/** the amount be subtracted by the index of a bit so that it can be shifted */
#define INDEX_SHIFT 7
/** the second index of a subkey to be fetched for the shift operation */
#define SECOND_INDEX 2
/** to divide an index in half in the fFunction function */
#define HALF 2

/**
 * @param data an array of bytes
 * @param idx the index of the bit to be examined
 * @return zero or one based on the value of the bit at index idx
 */
int getBit(byte const data[], int idx)
{   
    idx -= INDEX_DECREMENT; // changes indexing to start at 0
    int byte_pos = idx / BYTE_SIZE;
    int bit_pos = INDEX_SHIFT - (idx % BYTE_SIZE);

    return data[byte_pos] >> bit_pos & 1; // shifts over the correct byte by the bit position
}


/**
 * @param data an array of bytes
 * @param idx the index of the bit to be examined
 * @param val changes the purpose of the function (0 to clear, 1 to set)
 * clears or sets the bit at index idx of the data array
 */
void putBit(byte data[], int idx, int val)
{
    idx -= INDEX_DECREMENT; // changes to 0-based indexing
    int byte_pos = idx / BYTE_SIZE;
    int bit_pos = INDEX_SHIFT - (idx % BYTE_SIZE);

    if (val == 1) {
        data[byte_pos] |= (1 << bit_pos); // sets the bit at the index
    }
    else {
        data[byte_pos] &= ~(1 << bit_pos); // clears the bit at the index
    }
}

/**
 * @param output the output array of bits
 * @param input the input array of bits
 * @param perm an array used for the permutation
 * @param n the number of elements to examine in the perm array
 * performs the permumte operation, copying n bits from input to output (selected by the first n elements of perm)
 */
void permute(byte output[], byte const input[], int const perm[], int n)
{
    for (int i = 1; i <= n; i++) {       
        putBit(output, i, getBit(input, perm[i - INDEX_DECREMENT]));
    }

    if (n % BYTE_SIZE != 0) {
        for (int i = 1; i <= BYTE_SIZE - (n % BYTE_SIZE); i++) {
            putBit(output, n + i, 0);
        }
    }
}

/**
 * @param K an array of subkeys (array of an array of bits)
 * @param key the key to generate the list of subkeys from
 * computes 16 subkeys based on the input key and stores each one in an element of the K array
 */
void generateSubkeys(byte K[ ROUND_COUNT ][ SUBKEY_BYTES ], byte const key[ BLOCK_BYTES ])
{
    byte c[ROUND_COUNT][ROUND_TO_BYTES(HALF_SUBKEY_BITS + HALF_SUBKEY_BITS)] = {0};
    byte d[ROUND_COUNT][HALF_SUBKEY_BYTES] = {0};

    permute(c[0], key, leftSubkeyPerm, HALF_SUBKEY_BITS);
    permute(d[0], key, rightSubkeyPerm, HALF_SUBKEY_BITS);

    for (int i = 1; i < ROUND_COUNT; i++) {
        
        for (int j = 1; j <= HALF_SUBKEY_BITS - subkeyShiftSchedule[i]; j++) {
            putBit(c[i], j, getBit(c[i - 1], j + subkeyShiftSchedule[i]));
            putBit(d[i], j, getBit(d[i - 1], j + subkeyShiftSchedule[i]));
        }

        if (subkeyShiftSchedule[i] > 1) {
            putBit(c[i], HALF_SUBKEY_BITS - 1, getBit(c[i - 1], 1));
            putBit(c[i], HALF_SUBKEY_BITS, getBit(c[i - 1], SECOND_INDEX));
            putBit(d[i], HALF_SUBKEY_BITS - 1, getBit(d[i - 1], 1));
            putBit(d[i], HALF_SUBKEY_BITS, getBit(d[i - 1], SECOND_INDEX));
        }
        else {
            putBit(c[i], HALF_SUBKEY_BITS, getBit(c[i - 1], 1));
            putBit(d[i], HALF_SUBKEY_BITS, getBit(d[i - 1], 1));
        }

        for (int j = 1; j <= HALF_SUBKEY_BITS; j++) {
            putBit(c[i], HALF_SUBKEY_BITS + j, getBit(d[i], j));
        }

        permute(K[i], c[i], subkeyPerm,  SUBKEY_BITS);
    }
}

/**
 * @param output the result of an S-Box calculation // change
 * @param input the input to perform the calculation on // change
 * @param idx an index to help with the calculation
 * returns the result of an S-Box calculation in the four high-order bits of output[0]
 */
void sBox(byte output[1], byte const input[ SUBKEY_BYTES ], int idx)
{
    int b1 = getBit(input, idx * 6 + 1);
    int b2 = getBit(input, idx * 6 + 2);
    int b3 = getBit(input, idx * 6 + 3);
    int b4 = getBit(input, idx * 6 + 4);
    int b5 = getBit(input, idx * 6 + 5);
    int b6 = getBit(input, idx * 6 + 6);
    
    int row = (b1 << 1) | b6;
    int col = (b2 << 3) | (b3 << 2) | (b4 << 1) | b5;
    
    output[0] = sBoxTable[idx][row][col] << 4;
}

/**
 * @param result the result of the f function
 * @param R a 32-bit input value
 * @param K a subkey to be used in the function
 * computes the f function based on R and K and stores the result in the result array
 */
void fFunction(byte result[ HALF_BLOCK_BYTES ], byte const R[ HALF_BLOCK_BYTES ], byte const K[ SUBKEY_BYTES ])
{
    byte R_new[SUBKEY_BITS] = {0};
    byte B[SUBKEY_BITS] = {0};

    permute(R_new, R, expandedRSelector, SUBKEY_BITS);

    for (int i = 0; i < SUBKEY_BITS; i++) {
        B[i] = R_new[i] ^ K[i];
    }

    byte sbox_result[1] = {0};
    byte to_permute[HALF_BLOCK_BYTES] = {0};
    for (int i = 0; i < SBOX_COUNT; i++) {
        sBox(sbox_result, B, i);

        
        if (i % HALF == 0) {
            to_permute[i / HALF] = sbox_result[0];
        }
        else {
            to_permute[i / HALF] |= (sbox_result[0] >> SBOX_OUTPUT_BITS);
        }
    }

    permute(result, to_permute, fFunctionPerm, HALF_BLOCK_BITS);
}

/**
 * @param block the block to perform the encryption on
 * @param K an array of subkeys to be used in the encryption process
 * performs a single block encryption on the byte array in block, using the subkeys in the K array
 */
void encryptBlock(byte block[ BLOCK_BYTES ], byte const K[ ROUND_COUNT ][ SUBKEY_BYTES ])
{
    byte L[ROUND_COUNT][HALF_BLOCK_BYTES] = {0};
    byte R[ROUND_COUNT][HALF_BLOCK_BYTES] = {0};

    permute(L[0], block, leftInitialPerm, HALF_BLOCK_BITS);
    permute(R[0], block, rightInitialPerm, HALF_BLOCK_BITS);

    for (int i = 1; i < ROUND_COUNT; i++) {
        fFunction(R[i], R[i - 1], K[i]);
        
        for (int j = 0; j < HALF_BLOCK_BYTES; j++) {
            L[i][j] = R[i - 1][j];
            R[i][j] ^= L[i - 1][j];
        }
    }

    byte result[BLOCK_BYTES] = {0}; 
    for (int i = 0; i < BLOCK_BYTES; i++) {
        if (i >= HALF_BLOCK_BYTES) {
            result[i] = L[ROUND_COUNT - 1][i - HALF_BLOCK_BYTES];
        }
        else {
            result[i] = R[ROUND_COUNT - 1][i];
        }
    }

    permute(block, result, finalPerm, BLOCK_BITS);
}

/**
 * @param block the block to perform the decryption on
 * @param K an array of subkeys to be used in the decryption procecss
 * performs a single block decrypt operation on the byte array in block, using the subkeys in the K array
 */
void decryptBlock(byte block[ BLOCK_BYTES ], byte const K[ ROUND_COUNT ][ SUBKEY_BYTES ])
{
    byte L[ROUND_COUNT][HALF_BLOCK_BYTES] = {0};
    byte R[ROUND_COUNT][HALF_BLOCK_BYTES] = {0};

    permute(L[0], block, leftInitialPerm, HALF_BLOCK_BITS);
    permute(R[0], block, rightInitialPerm, HALF_BLOCK_BITS);

    for (int i = 1; i < ROUND_COUNT; i++) {
        fFunction(R[i], R[i - 1], K[ROUND_COUNT - i]);
        
        for (int j = 0; j < HALF_BLOCK_BYTES; j++) {
            L[i][j] = R[i - 1][j];
            R[i][j] ^= L[i - 1][j];
        }
    }

    byte result[BLOCK_BYTES] = {0}; 
    for (int i = 0; i < BLOCK_BYTES; i++) {
        if (i >= HALF_BLOCK_BYTES) {
            result[i] = L[ROUND_COUNT - 1][i - HALF_BLOCK_BYTES];
        }
        else {
            result[i] = R[ROUND_COUNT - 1][i];
        }
    }

    permute(block, result, finalPerm, BLOCK_BITS);
}

byte *encryptTDES(byte input[], int inputLen, byte key[], int keyLen, int *n)
{
    int add = BYTE_SIZE - (inputLen % BYTE_SIZE);
    byte padded_input[inputLen + add];

    memcpy(padded_input, input, inputLen);
    memset(padded_input + inputLen, add, add);

    byte K1[ROUND_COUNT][SUBKEY_BYTES] = {0};
    byte K2[ROUND_COUNT][SUBKEY_BYTES] = {0};
    byte K3[ROUND_COUNT][SUBKEY_BYTES] = {0};

    generateSubkeys(K1, key);
    generateSubkeys(K2, key + BLOCK_BYTES);
    generateSubkeys(K3, key + (BLOCK_BYTES + BLOCK_BYTES));

    int num = (inputLen + add) / BLOCK_BYTES;
    byte block[BLOCK_BYTES] = {0};
    byte *result = (byte *) malloc(sizeof(byte) * BLOCK_BYTES * num);

    for (int i = 0; i < num; i++) {
        memcpy(block, padded_input + (i * BLOCK_BYTES), BLOCK_BYTES);

        encryptBlock(block, K1);
        decryptBlock(block, K2);
        encryptBlock(block, K3);

        memcpy(result + (i * BLOCK_BYTES), block, BLOCK_BYTES);
    }
    
    *n = inputLen + add;
    return result;
}

byte *decryptTDES(byte input[], int inputLen, byte key[], int keyLen, int *n)
{

    if (!(inputLen / BYTE_SIZE > 0 && inputLen % BYTE_SIZE == 0)) {
        fprintf(stderr, "Invalid encrypted data length\n");
        exit(EXIT_FAILURE);
    }

    byte K1[ROUND_COUNT][SUBKEY_BYTES] = {0};
    byte K2[ROUND_COUNT][SUBKEY_BYTES] = {0};
    byte K3[ROUND_COUNT][SUBKEY_BYTES] = {0};

    generateSubkeys(K1, key);
    generateSubkeys(K2, key + BLOCK_BYTES);
    generateSubkeys(K3, key + (BLOCK_BYTES + BLOCK_BYTES));

    int num = inputLen / BLOCK_BYTES;
    byte block[BLOCK_BYTES] = {0};
    byte *result = (byte *) malloc(sizeof(byte) * BLOCK_BYTES * num);

    for (int i = 0; i < num; i++) {
        memcpy(block, input + (i * BLOCK_BYTES), BLOCK_BYTES);

        decryptBlock(block, K3);
        encryptBlock(block, K2);
        decryptBlock(block, K1);

        memcpy(result + (i * BLOCK_BYTES), block, BLOCK_BYTES);
    }

    int padding = result[inputLen - 1];

    result = (byte *) realloc(result, inputLen - padding);

    *n = inputLen - padding;
    return result;
}
