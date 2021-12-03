#ifndef FLBWT_H
#define FLBWT_H

#ifdef __cplusplus
extern "C"
{
#endif

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
    uint8_t *bwt_string(const uint8_t *T, uint64_t n);

#ifdef __cplusplus
}
#endif

#endif