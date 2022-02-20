#ifndef FLBWT_INDUCE64BIT_HPP
#define FLBWT_INDUCE64BIT_HPP

#include <stdint.h>
#include "container.hpp"
#include "induce32bit.hpp"

namespace flbwt
{
    /**
     * @brief Function for inducing the BWT for the original input string T.
     */
    flbwt::BWT_result *induce_bwt_64bit(int64_t *SA, flbwt::Container *container);
}

#endif