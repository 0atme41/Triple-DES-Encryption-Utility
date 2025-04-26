/** 
    @file TDESinternal.h
    @author dbsturgi
    Definitions and declarations that would normally be part of the
    TDES.c implementation file.  They're exposed in this header so the
    unit test program can access them to test internal parts of the
    TDES implementation.
*/

#include "IO.h"
#include "magic.h"

/** Number of bits in a byte. */
#define BYTE_SIZE 8

/** Round a number of bits up to the nearest number of bytes needed 
    to store that many bits. */
#define ROUND_TO_BYTES( bits ) (((bits) + BYTE_SIZE - 1)/BYTE_SIZE)

/** Number of bytes in a DES block. */
#define BLOCK_BYTES ROUND_TO_BYTES( BLOCK_BITS )

/** Number of bytes in the left or right halves of a block (L and R). */
#define HALF_BLOCK_BYTES ROUND_TO_BYTES( HALF_BLOCK_BITS )

/** Number of bytes to store the left-side and right-side values (C
    and D) used to create the subkeys. */
#define HALF_SUBKEY_BYTES ROUND_TO_BYTES( HALF_SUBKEY_BITS )

/** Number of bytes to store a whole subkey (K_1 .. K_16). */
#define SUBKEY_BYTES ROUND_TO_BYTES( SUBKEY_BITS )

//
// Don't comment these functions here. They're not really part of the
// public interface for TDES.c.  Comment them in the implementation file
// instead.
//
int getBit( byte const data[], int idx );
void putBit( byte data[], int idx, int val );
void permute( byte output[], byte const input[], int const perm[], int n );
void generateSubkeys( byte K[ ROUND_COUNT ][ SUBKEY_BYTES ], byte const key[ BLOCK_BYTES ] );
void sBox( byte output[ 1 ], byte const input[ SUBKEY_BYTES ], int idx );
void fFunction( byte result[ HALF_BLOCK_BYTES ], byte const R[ HALF_BLOCK_BYTES ],
                byte const K[ SUBKEY_BYTES ] );
void encryptBlock( byte block[ BLOCK_BYTES ],
                   byte const K[ ROUND_COUNT ][ SUBKEY_BYTES ] );
void decryptBlock( byte block[ BLOCK_BYTES ],
                   byte const K[ ROUND_COUNT ][ SUBKEY_BYTES ] );
