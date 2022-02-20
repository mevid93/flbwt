#ifndef FLBWT_INDUCE40BIT_HPP
#define FLBWT_INDUCE40BIT_HPP

#include <stdint.h>
#include "container.hpp"
#include "induce32bit.hpp"

namespace flbwt
{
    /**
     * @brief Function for inducing the BWT for the original input string T.
     */
    flbwt::BWT_result *induce_bwt_40bit(uint32_t *SA_L, int8_t *SA_U, flbwt::Container *container);
}

#endif