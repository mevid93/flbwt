#ifndef FLBWT_UTILITY_HPP
#define FLBWT_UTILITY_HPP

#include <stdint.h>

/**
 * @brief Calculate the alphabet size of the input string.
 * 
 * @param T input string
 * @param n input string lentgth
 * @return uint8_t alphabet size
 */
uint8_t calculate_alphabet_size(const uint8_t *T, const uint64_t n);

/**
 * @brief Fast algorithm to calculate log2 for 64-bit integer.
 * 
 * @param value 64-bit input integer
 * @return int result of log2
 */
int log2_64(uint64_t value);

#endif