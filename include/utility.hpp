#ifndef FLBWT_UTILITY_HPP
#define FLBWT_UTILITY_HPP

#include <stdint.h>

namespace flbwt {

/**
 * @brief Fast algorithm to calculate log2 for unsigned 64-bit integer.
 * 
 * @param value 64-bit unsigned integer
 * @return uint8_t result of log2
 */
uint8_t log2_64(uint64_t value);

/**
 * @brief Get the position of most significant bit of 64-bit unsigned integer.
 * 
 * @param n 64-bit unsigned integer
 * @return uint8_t msb bit position
 */
uint8_t position_of_msb(uint64_t n);

/**
 * @brief Get the maximum unsigned integer that can be
 * presented with n bits.
 * 
 * @param n number of bits
 * @return uint64_t maximum value
 */
uint64_t max_integer(uint8_t n);

}

#endif