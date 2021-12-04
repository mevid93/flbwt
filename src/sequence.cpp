#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "sequence.hpp"
#include "utility.hpp"

Sequence::Sequence(const uint64_t n)
{
    /* allocate memory for the upper stream */
    uint64_t us_bits = 2 * n;                  

    this->upper_stream = new sdsl::bit_vector(n);

    this->upper_stream_length = us_bits;

    /* allocate memory for the lower stream */
    uint64_t ls_bits = n * (log2_64(0xffffffffffffffff / n + 1) + 1);
    uint64_t num_of_int64s = ls_bits / 64 + 1;

    this->lower_stream = (uint64_t *)malloc(num_of_int64s * sizeof(uint64_t));

    if (this->lower_stream == NULL)
    {
        throw std::runtime_error("lower_stream malloc failed(): Could not allocate memory");
    }

    this->lower_stream_length = ls_bits;
}

void Sequence::store_integer(const uint64_t index, const uint64_t integer)
{
}

uint64_t Sequence::get_integer(const uint64_t index)
{
    return 0;
}

Sequence::~Sequence()
{
    /* free resources from upper stream */
    delete this->upper_stream;
    this->upper_stream_length = 0;

    /* free resources from lower stream */
    free(this->lower_stream);
    this->lower_stream = NULL;
    this->lower_stream_length = 0;
}