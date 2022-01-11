#include <algorithm>
#include "packed_array.hpp"

flbwt::PackedArray::PackedArray(uint64_t n, uint8_t w)
{
    this->n = n;
    this->w = w;

    uint64_t x = (n / PBS) * w + ((n % PBS) * w + PBS - 1) / PBS;
    this->b = new uint32_t[x + 1];

    std::fill_n(this->b, x + 1, 0);
}

void flbwt::PackedArray::pa_set(uint64_t i, uint64_t x)
{
    uint32_t *b = this->b + (i >> _logD_) * this->w;
    i = (i % _D_) * this->w;
    this->set_bits(b, i, x);
}

void flbwt::PackedArray::set_bits(uint32_t *b, uint64_t i, uint64_t x)
{
    int d = this->w;
    uint64_t m;
    uint64_t y;
    int64_t d2;

    b += (i >> _logD_);
    i %= _D_;

    while (i + d > _D_)
    {
        d2 = _D_ - i;
        y = x >> (d - d2);
        m = (1 << d2) - 1;
        *b = (*b & (~m)) | y;
        b++;
        i = 0;
        d -= d2;
        x &= (1 << d) - 1;
    }

    m = (1 << d) - 1;
    y = x << (_D_ - i - d);
    *b = (*b & (~m)) | y;
}

uint64_t flbwt::PackedArray::pa_get(uint64_t i)
{
    uint32_t *b;

    b = this->b + (i >> _logD_) * this->w;
    i = (i % _D_) * this->w;

    return this->get_bits(b, i);
}

uint64_t flbwt::PackedArray::get_bits(uint32_t *b, uint64_t i)
{
    int64_t d = this->w;
    uint64_t x;
    uint64_t z;

    b += (i >> _logD_);
    i &= (_D_ - 1);
    
    if (i + d <= 2 * _D_)
    {
        x = ((uint64_t)b[0] << _D_) + b[1];
        x <<= i;
        x >>= (_D_ * 2 - 1 - d);
        x >>= 1;
    }
    else
    {
        x = ((uint64_t)b[0] << _D_) + b[1];
        z = (x << _D_) + b[2];
        x <<= i;
        x &= ((1L << _D_) - 1) << _D_;
        z <<= i;
        z >>= _D_;
        x += z;
        x >>= (2 * _D_ - d);
    }

    return x;
}

flbwt::PackedArray::~PackedArray()
{
    delete[] this->b;
    this->b = NULL;
}
