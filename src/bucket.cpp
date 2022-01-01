#include "bucket.hpp"
#include "utility.hpp"

flbwt::Bucket::Bucket(const uint8_t c, const uint8_t lsi_bits, const uint8_t ss_limit)
{
    this->c = c;
    this->bits = 0;
    this->strings = 0;
    this->next_position = 0;
    this->b = new sdsl::bit_vector(0, 0);
    this->LSI_BITS = lsi_bits;
    this->SS_LIMIT = ss_limit;
}

void flbwt::Bucket::insert_substring(const uint8_t *T, const uint64_t p, const uint64_t l)
{
    this->strings++;

    // in case of short substring
    if (l < this->SS_LIMIT)
    {
        this->bits += l * 8;
        this->b->bit_resize(this->bits);
        for (uint8_t i = 0; i < l; i++)
        {
            this->b->set_int(this->next_position, T[p + i], 8);
            this->next_position += 8;
        }

        return;
    }

    // long substring
    this->bits += LSI_BITS;
    this->b->bit_resize(this->bits);
    this->b->set_int(this->next_position, p, LSI_BITS);
    this->next_position += LSI_BITS;
}

void flbwt::Bucket::reset_next_position()
{
    this->next_position = 0;
}

flbwt::Bucket::~Bucket()
{
    this->bits = 0;
    this->strings = 0;
    this->next_position = 0;
    delete this->b;
}