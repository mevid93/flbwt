#ifndef FLBWT_SAIS48BIT_HPP
#define FLBWT_SAIS48BIT_HPP

#include <stdint.h>
#include "packed_array.hpp"

/*
 * Modified version of the Yuta Mori's SAIS implementation.
 */

namespace flbwt
{

    inline void set_48bit_value(uint32_t *L, int16_t *U, int64_t index, int64_t value)
    {
        U[index] = value >> 32;
        L[index] = value & 0xffffffff;
    }

    inline int64_t get_48bit_value(uint32_t *L, int16_t *U, int64_t index)
    {
        return (int64_t)U[index] << 32 | L[index];
    }

    /**
     * @brief Induced sorting for sorting suffixes (48 bit integers).
     * 
     * @param T input string (used in first level of recursion)
     * @param TA_L input string (lower 32 bits)
     * @param TA_U input string (upper 16 bits)
     * @param SA_L suffix array lower stream (first 32 bits)
     * @param SA_U suffix array upper stream (last 16 bits)
     * @param fs 
     * @param n input string length
     * @param k alphabet size
     * @param cs width of input string elements
     */
    void sais_48bit(const uint8_t *T, uint32_t *TA_L, int16_t *TA_U, uint32_t *SA_L,
                    int16_t *SA_U, uint64_t fs, uint64_t n, uint64_t k, uint8_t cs);

}

#endif