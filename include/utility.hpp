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

extern uint64_t MAX_MEM_USE;
extern uint64_t CUR_MEM_USE;

/**
 * @brief Increase memory counter with n bytes.
 * 
 * @param n number of bytes
 */
void increase_memory_allocation(uint64_t n);

/**
 * @brief Decrease memory counter with n bytes.
 * 
 * @param n number of bytes
 */
void decrease_memory_allocation(uint64_t n);

}

#endif