#ifndef FLBWT_INDUCE_HPP
#define FLBWT_INDUCE_HPP

#include <stdint.h>
#include "container.hpp"
#include "packed_array.hpp"

namespace flbwt
{
    struct BWT_result 
    {
        uint64_t last;
        uint8_t *BWT;
    };

    /**
     * @brief Function for inducing the BWT for the original input string T.
     */
    flbwt::BWT_result *induce_bwt(flbwt::PackedArray *SA, flbwt::Container *container);
}

#endif