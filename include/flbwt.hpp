#ifndef FLBWT_HPP
#define FLBWT_HPP

#include <stdint.h>
#include "container.hpp"
#include "packed_array.hpp"

namespace flbwt
{

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
 * @param free_T free input string when not needed anymore (more efficient)
 * @return uint8_t* result string
 */
uint8_t *bwt_string(uint8_t *T, const uint64_t n, bool free_T);

// REST OF THE FUNCTIONS ARE NOT MEANT FOR THE USER (ONLY FOR TESTING)

/**
 * @brief Function for extracting S* substrings from the input string.
 * However, the last S* substring T(n) will be ignored.
 * 
 * @param T input string
 * @param n length of the input string
 * @param k alphabet size
 * @return container
 */
flbwt::Container *extract_LMS_strings(uint8_t *T, const uint64_t n);

/**
 * @brief Function for sorting LMS substrings. 
 * 
 * @param T input string
 * @param container container where substrings are stored
 */
uint8_t **sort_LMS_strings(uint8_t *T, flbwt::Container *container);

/**
 * @brief Create a shortened string T1.
 */
flbwt::PackedArray *create_shortened_string(uint8_t *T, const uint64_t n, flbwt::Container *container);

}

#endif