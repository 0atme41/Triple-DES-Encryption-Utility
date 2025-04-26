#include "IO.h"

/** the initial size of the buffer to read into */
#define INITIAL_SIZE 1024
/** the factor to grow the buffer by when reading in values */
#define GROW_FACTOR 2

byte *readFile(char const *filename, int *n) 
{
    *n = 0;
    int capacity = INITIAL_SIZE;
    
    FILE *fp = fopen(filename, "rb");

    if (!fp) {
        perror(filename);
        exit(EXIT_FAILURE);
    }
    
    byte *contents = (byte *) malloc(capacity); // makes the initial buffer
    
    int amount_read;
    while ((amount_read = fread(contents + *n, 1, INITIAL_SIZE, fp)) > 0) {
        *n += amount_read; // updates the length
        if (*n + INITIAL_SIZE > capacity) { // resizes if needed
            capacity *= GROW_FACTOR;
            contents = (byte *) realloc(contents, capacity);
        }
    }

    fclose(fp);
    return contents;
}

bool writeFile(char const *filename, byte *data, int n)
{
    FILE *fp = fopen(filename, "wb");

    if (!fp) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    if (fwrite(data, sizeof(byte), n, fp) != n) {
        fclose(fp);
        return false;
    }
    fclose(fp);
    return true;
}
