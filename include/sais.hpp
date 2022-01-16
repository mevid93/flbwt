#ifndef FLBWT_SAIS_HPP
#define FLBWT_SAIS_HPP

#include <stdint.h>
#include "sdsl/bit_vectors.hpp"

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
 * @param SA_signs bit_vector holding information of signs of SA elements
 * @param fs 
 * @param n input string length
 * @param k alphabet size
 * @param T_width T character width in bits
 * @param SA_width SA character widht in bits
 */
    void sais_main(sdsl::bit_vector *T, uint64_t T_base, sdsl::bit_vector *SA, sdsl::bit_vector *SA_signs,
                   uint64_t fs, uint64_t n, uint64_t k, uint8_t T_width, uint8_t SA_width);

}

#endif