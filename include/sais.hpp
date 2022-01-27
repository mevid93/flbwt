#ifndef FLBWT_SAIS_HPP
#define FLBWT_SAIS_HPP

#include <stdint.h>
#include "packed_array.hpp"

/*
 * Modified version of the Yuta Mori's SAIS implementation.
 */

namespace flbwt
{

    /**
     * @brief Induced sorting for sorting suffixes.
     * 
     * @param T input string
     * @param T_base input string starting offset
     * @param SA suffix array
     * @param fs 
     * @param n input string length
     * @param k alphabet size
     */
    void sais_main(flbwt::PackedArray *T, uint64_t T_base, flbwt::PackedArray *SA,
                   uint64_t fs, uint64_t n, uint64_t k);

}

#endif