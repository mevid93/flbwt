#ifndef FLBWT_HPP
#define FLBWT_HPP

#include <stdint.h>
#include "container.hpp"

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
 * @return uint8_t* result string
 */
uint8_t *bwt_string(const uint8_t *T, const uint64_t n);

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
flbwt::Container *extract_LMS_strings(const uint8_t *T, const uint64_t n, const uint8_t k);

/**
 * @brief Function for counting the number bits required to store S* substrings
 * in each c-bucket. This function ignores the last S* substring T(n).
 * 
 * @param T input string
 * @param container container object
 * @return bit counts
 */
uint64_t *count_bits_for_buckets(const uint8_t *T, Container *container);

}

#endif