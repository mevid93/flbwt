#ifndef FLBWT_UTILITY_HPP
#define FLBWT_UTILITY_HPP

#include <stdint.h>

namespace flbwt {

/**
 * @brief Get the position of most significant bit of 64-bit unsigned integer.
 * 
 * @param n 64-bit unsigned integer
 * @return uint8_t msb bit position
 */
inline uint8_t position_of_msb(uint64_t n)
{
    return 64 - __builtin_clzl(n); 
}

}

#endif