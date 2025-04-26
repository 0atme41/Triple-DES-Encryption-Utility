
#include "TDES.h"
#include "IO.h"

/** the minimum amount of arguments */
#define MIN_ARGS 4
/** the maximum amount of arguments */
#define MAX_ARGS 5
/** the length that the key should be */
#define KEY_LENGTH 24
/** argument number for the input file */
#define INPUT_FILE 2
/** argument number for the output file */
#define OUTPUT_FILE 3


int main(int argc, char *argv[]) 
{
    
    if (argc < MIN_ARGS || argc > MAX_ARGS || (argc == MAX_ARGS && (strcmp(argv[1], "-d") != 0)) || (argc == MIN_ARGS && (strcmp(argv[1], "-d") == 0))) {
        fprintf(stderr, "usage: tcrypt [-d] KEY_FILE INPUT_FILE OUTPUT_FILE\n");
        exit(EXIT_FAILURE);
    }

    int n;
    int keyLen;
    int inputLen;
    byte *input;
    byte *key;
    byte *output;

    if (argc == MIN_ARGS) {
        key = readFile(argv[1], &keyLen);

        if (keyLen != KEY_LENGTH) {
            fprintf(stderr, "Invalid key length\n");
            exit(EXIT_FAILURE);
        }

        input = readFile(argv[INPUT_FILE], &inputLen);
        output = encryptTDES(input, inputLen, key, keyLen, &n);

        if (!writeFile(argv[OUTPUT_FILE], output, n)) {
            fprintf(stderr, "Something went wrong writing the encryption\n");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    }

    key = readFile(argv[INPUT_FILE], &keyLen);

    if (keyLen != KEY_LENGTH) {
        fprintf(stderr, "Invalid key length\n");
        exit(EXIT_FAILURE);
    }

    input = readFile(argv[INPUT_FILE + 1], &inputLen);
    output = decryptTDES(input, inputLen, key, keyLen, &n);

    if (!writeFile(argv[OUTPUT_FILE + 1], output, n)) {
        fprintf(stderr, "Something went wrong writing the decryption\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
