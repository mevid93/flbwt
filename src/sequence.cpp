#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "sdsl/util.hpp"
#include "sequence.hpp"
#include "utility.hpp"

Sequence::Sequence(const uint64_t n)
{
    /* allocate memory for the upper stream */
    uint64_t us_bits = 2 * n;
    this->upper_stream = new sdsl::bit_vector(us_bits, 0);
    this->upper_stream_length = us_bits;

    /* allocate memory for the lower stream */
    uint64_t ls_bits = n * (log2_64(Sequence::U / n + 1) + 1);
    this->lower_stream = new sdsl::bit_vector(ls_bits, 0);
    this->lower_stream_length = ls_bits;

    /* set number of integers that can be stored to sequence */
    this->n = n;

    /* calculate the number of bits considering each stream */
    uint8_t log2result = log2_64(n);

    this->ubits = log2result;
    this->lbits = 64 - log2result;

    /* initialize the auxiliary datatype as null */
    this->sls = NULL;
}

void Sequence::store_integer(const uint64_t index, const uint64_t integer)
{
    this->select_support_enabled = false;

    /* store integer upper stream bits */
    uint64_t shifted = integer >> (64 - this->ubits);
    uint64_t us_bit_index = index + shifted;
    (*this->upper_stream)[us_bit_index] = 1;

    /* store integer lower stream bits */
    uint64_t ls_index = (index - 1) * this->lbits + 1;
    this->lower_stream->set_int(ls_index, integer, this->lbits);
}

uint64_t Sequence::get_integer(const uint64_t index)
{
    if (!this->select_support_enabled)
    {
        this->enable_select_operation();
    }

    /* get upper stream part */
    uint64_t upper_bits = (*this->sls)(index)-index;
    upper_bits = upper_bits << (64 - this-> ubits);

    /* get the lower stream part */
    uint64_t ls_index = (index - 1) * this->lbits + 1;
    uint64_t lower_bits = this->lower_stream->get_int(ls_index, this->lbits);

    /* combine the parts to form the actual integer */
    uint64_t integer = upper_bits | lower_bits;

    return integer;
}

Sequence::~Sequence()
{
    /* free resources from upper stream */
    delete this->upper_stream;
    this->upper_stream_length = 0;

    /* free resources from lower stream */
    delete this->lower_stream;
    this->lower_stream_length = 0;

    /* reset the number of integers that can be stored */
    this->n = 0;

    /* free auxiliary data structure */
    if (this->sls != NULL)
    {
        delete this->sls;
    }

    /* reset flags */
    this->select_support_enabled = false;
}

void Sequence::enable_select_operation()
{
    this->select_support_enabled = true;

    this->sls = new sdsl::select_support_mcl<1>(this->upper_stream);
}