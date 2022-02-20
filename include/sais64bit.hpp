#ifndef FLBWT_SAIS64BIT_HPP
#define FLBWT_SAIS64BIT_HPP

#include <stdint.h>
#include "packed_array.hpp"

/*
 * Modified version of the Yuta Mori's SAIS implementation.
 */

namespace flbwt
{
    /**
     * @brief Induced sorting for sorting suffixes (64 bit integers).
     * 
     * @param T input string
     * @param SA suffix array
     * @param fs 
     * @param n input string length
     * @param k alphabet size
     * @param cs width of input string elements
     */
    void sais_64bit(const uint8_t *T, int64_t *SA, uint64_t fs, uint64_t n, uint64_t k, uint8_t cs);

}

#endif