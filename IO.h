/** 
    @file IO.h
    @author
    Functions to make it easy to read and write binary files.
*/

#ifndef _IO_H_
#define _IO_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Type used to represent a byte. */
typedef unsigned char byte;

#endif

/**
 * @param filename the name of the file to be read from
 * @param n a pass-by-reference parameter that stores the length of the file
 * @return a pointer to a dynamically allocated array of bytes with the contents of the file
 * reads the contents of the file with the given filename
 */
byte *readFile(char const *filename, int *n);

/**
 * @param filename the name of the file to write to
 * @param data an array of bytesto write
 * @param n the amount of bytes to write
 * @return true if successful and false if not
 * this function is for writing bytes in binary
 */
bool writeFile(char const *filename, byte *data, int n);