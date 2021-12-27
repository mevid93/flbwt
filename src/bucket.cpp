#include "bucket.hpp"
#include "utility.hpp"

flbwt::Bucket::Bucket(const uint8_t c, const uint64_t n)
{
    this->c = c;
    this->bits = n;
    this->strings = 0;
    this->next_position = new uint64_t(0);
    this->b = new sdsl::bit_vector(n, 0);
}

void flbwt::Bucket::insert_substring(const uint8_t *T, const uint64_t n, const uint8_t k, const uint64_t p, const uint64_t l)
{
    uint64_t LSI_BITS;          // number of bits required to store long substring index
    uint64_t LSL_BITS;          // number of bits required to store long substring length
    uint64_t SS_LIMIT;          // small substring length limit
    uint64_t SS_BITS;           // number of bits requires to store small substring length

    LSI_BITS = flbwt::position_of_msb(n);
    LSL_BITS = LSI_BITS;
    SS_LIMIT = flbwt::log2_64(n) / flbwt::log2_64(k);
    SS_BITS = flbwt::position_of_msb(SS_LIMIT);

    if (l <= SS_LIMIT)
    {
        // set type
        (*this->next_position)++;
        // set length
        this->b->set_int(*this->next_position, l, SS_BITS);
        (*this->next_position) += SS_BITS;
        // set characters
        for(uint64_t i = 0; i < l; i++)
        {
            this->b->set_int(*this->next_position, T[p + i], 8);
            (*this->next_position) += 8;
        }
    }
    else
    {
        // set type
        (*this->b)[*this->next_position] = 1;
        (*this->next_position)++;
        // set index
        this->b->set_int(*this->next_position, p, LSI_BITS);
        (*this->next_position) += LSI_BITS;
        // set length
        this->b->set_int(*this->next_position, l, LSL_BITS);
        (*this->next_position) += LSI_BITS;
    }

    this->strings++;
}

flbwt::Bucket::~Bucket()
{
    this->bits = 0;
    this->strings = 0;
    delete this->next_position;
    delete this->b;
}