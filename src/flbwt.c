#ifndef FLBWT_H
#define FLBWT_H

#include <stdint.h>

// API FOR THE USER

/**
 * @brief Function for performing Burrows-Wheeler Transform for 
 * the input file and writing the result to the output file.
 * 
 * @param input_filename filename (path) of the input file
 * @param output_filename filename (path) of the output file
 */
void bwt_file(char *input_filename, char *output_filename);

/**
 * @brief Function for performing Burrows-Wheeler Transform for
 * the input string and returning the result.
 * 
 * @param T input string
 * @return uint8_t* result string
 */
uint8_t *bwt_string(uint8_t *T);

// API FOR THE GOOGLE TEST FRAMEWORK (USER SHOULD NOT CALL THIS)

#endif