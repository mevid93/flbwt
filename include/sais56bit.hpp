#ifndef FLBWT_SAIS56BIT_HPP
#define FLBWT_SAIS56BIT_HPP

#include <stdint.h>
#include "packed_array.hpp"

/*
 * Modified version of the Yuta Mori's SAIS implementation.
 */

namespace flbwt
{

    inline void set_56bit_value(uint32_t *L, uint16_t *M, int8_t *U, int64_t index, int64_t value)
    {
        U[index] = value >> 48;
        M[index] = value >> 32;
        L[index] = value & 0xffffffff;
    }

    inline int64_t get_56bit_value(uint32_t *L, uint16_t *M, int8_t *U, int64_t index)
    {
        return (int64_t)U[index] << 48 | (int64_t)M[index] << 32 | L[index];
    }

    /**
     * @brief Induced sorting for sorting suffixes (48 bit integers).
     * 
     * @param T input string (used in first level of recursion)
     * @param TA_L input string (lower 32 bits)
     * @param TA_M input string (middle 16 bits)
     * @param TA_U input string (upper 8 bits)
     * @param SA_L suffix array lower stream (first 32 bits)
     * @param SA_M suffix array middle stream (middle 16 bits)
     * @param SA_U suffix array upper stream (last 8 bits)
     * @param fs 
     * @param n input string length
     * @param k alphabet size
     * @param cs width of input string elements
     */
    void sais_56bit(const uint8_t *T, uint32_t *TA_L, uint16_t *TA_M, int8_t *TA_U, uint32_t *SA_L,
                    uint16_t *SA_M, int8_t *SA_U, uint64_t fs, uint64_t n, uint64_t k, uint8_t cs);

}

#endif