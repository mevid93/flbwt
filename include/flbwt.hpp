#ifndef FLBWT_HPP
#define FLBWT_HPP

#include <stdint.h>

/**
 * @brief Function for performing Burrows-Wheeler Transform for 
 * the input file and writing the result to the output file.
 * 
 * @param input_filename filename (path) of the input file
 * @param output_filename filename (path) of the output file
 */
void bwt_file(const char *input_filename, const char *output_filename);

/**
 * @brief Function for performing Burrows-Wheeler Transform for
 * the input string and returning the result. The result string is
 * dynamically allocated, so remember to free it in order to avoid
 * memory leaks.
 * 
 * @param T input string
 * @param n length of the input string
 * @return uint8_t* result string
 */
uint8_t *bwt_string(const uint8_t *T, const uint64_t n);

// REST OF THE FUNCTIONS ARE NOT MEANT FOR THE USER (ONLY FOR TESTING)

typedef struct LMS_strings
{
    uint64_t count;
} LMS_strings;

/**
 * @brief Function for extracting S* substrings from the input string.
 * However, the last S* substring T(n) will be ignored.
 * 
 * @param T input string
 * @param n length of the input string
 * @return LMS_strings
 */
LMS_strings *extract_LMS_strings(const uint8_t *T, const uint64_t n);

#endif