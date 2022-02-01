#ifndef FLBWT_INDUCE_HPP
#define FLBWT_INDUCE_HPP

#include <stdint.h>
#include "container.hpp"
#include "packed_array.hpp"

namespace flbwt
{
    /**
     * @brief Function for inducing the BWT for the original input string T.
     */
    void induce_bwt(flbwt::PackedArray *SA, flbwt::Container *container);
}

#endif