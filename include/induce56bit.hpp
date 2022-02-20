#ifndef FLBWT_INDUCE56BIT_HPP
#define FLBWT_INDUCE56BIT_HPP

#include <stdint.h>
#include "container.hpp"
#include "induce32bit.hpp"

namespace flbwt
{
    /**
     * @brief Function for inducing the BWT for the original input string T.
     */
    flbwt::BWT_result *induce_bwt_56bit(uint32_t *SA_L, uint16_t *SA_M, int8_t *SA_U, flbwt::Container *container);
}

#endif