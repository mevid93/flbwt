#ifndef FLBWT_INDUCE32BIT_HPP
#define FLBWT_INDUCE32BIT_HPP

#include <stdint.h>
#include "container.hpp"
#include "utility.hpp"

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
    flbwt::BWT_result *induce_bwt_32bit(int32_t *SA, flbwt::Container *container);
}

#endif