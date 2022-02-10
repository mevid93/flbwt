#include <cstdlib>
#include <stddef.h>
#include "packed_array.hpp"
#include "utility.hpp"

flbwt::PackedArray::PackedArray(uint64_t length, uint64_t max_integer, bool support_negative_integers)
{
    this->length = length;
    this->negative_integers = support_negative_integers;
    this->integer_bits = flbwt::position_of_msb(max_integer);

    // Allocate space for integers (excluding sign bits)
    uint64_t bits_required = length * this->integer_bits;
    uint64_t arr_length = bits_required / 64 + 1;
    this->arr = (uint64_t *)malloc(arr_length * sizeof(uint64_t));
    this->arr_length = arr_length;

    // If negative numbers supported, allocate space for sign bits
    if (!this->negative_integers)
    {
        this->signs = NULL;
        this->signs_length = 0;
        return;
    }

    bits_required = length;
    uint64_t signs_length = bits_required / 64 + 1;
    this->signs = (uint64_t *)malloc(signs_length * sizeof(uint64_t));
    this->signs_length = signs_length;
}

flbwt::PackedArray::PackedArray(uint64_t length, bool support_negative_integers, uint8_t integer_bits)
{
    this->length = length;
    this->negative_integers = support_negative_integers;
    this->integer_bits = integer_bits;

    // Allocate space for integers (excluding sign bits)
    uint64_t bits_required = length * this->integer_bits;
    uint64_t arr_length = bits_required / 64 + 1;
    this->arr = (uint64_t *)malloc(arr_length * sizeof(uint64_t));
    this->arr_length = arr_length;

    // If negative numbers supported, allocate space for sign bits
    if (!this->negative_integers)
    {
        this->signs = NULL;
        this->signs_length = 0;
        return;
    }

    bits_required = length;
    uint64_t signs_length = bits_required / 64 + 1;
    this->signs = (uint64_t *)malloc(signs_length * sizeof(uint64_t));
    this->signs_length = signs_length;
}

uint64_t flbwt::PackedArray::get_length()
{
    return this->length;
}

uint8_t flbwt::PackedArray::get_integer_bits()
{
    return this->integer_bits;
}

bool flbwt::PackedArray::supports_negative_integers()
{
    return this->negative_integers;
}

void flbwt::PackedArray::set_value(uint64_t index, int64_t value)
{
    uint64_t abs_value = value;

    // Set the sign bit first, if supported
    if (this->negative_integers)
    {
        uint64_t arr_index = index / 64;
        uint64_t bit_index = 63 - (index % 64);

        if (value >= 0)
        { // Positive value --> clear sign bit
            this->signs[arr_index] &= ~(1UL << bit_index);
        }
        else
        { // Negative value --> set sign bit
            abs_value = -value;
            this->signs[arr_index] |= (1UL << bit_index);
        }
    }

    // Calculate index in value array
    uint64_t arr_index = this->integer_bits * index / 64;
    uint64_t bit_index = 63 - ((this->integer_bits * index) % 64);

    uint64_t bitmask1 = BIT_MASK(uint64_t, bit_index + 1);
    uint64_t bitmask2;
    int8_t shift;

    if (bit_index + 1 >= this->integer_bits)
    { // Enough space --> no need to split
        bitmask2 = BIT_MASK(uint64_t, bit_index + 1 - this->integer_bits);
        bitmask1 = bitmask1 & (~bitmask2);
        shift = bit_index + 1 - this->integer_bits;
        this->arr[arr_index] = (this->arr[arr_index] & (~bitmask1)) | (abs_value << shift);
    }
    else
    { // Not enough space --> splitting needed
        uint8_t wfp = bit_index + 1;
        uint8_t wsp = this->integer_bits - wfp;
        this->arr[arr_index] = (this->arr[arr_index] & (~bitmask1)) | (abs_value >> wsp);
        bitmask2 = 0xffffffffffffffff << (64 - wsp);
        this->arr[arr_index + 1] = (this->arr[arr_index + 1] & (~bitmask2)) | (abs_value << (64 - wsp));
    }
}

int64_t flbwt::PackedArray::get_value(uint64_t index)
{
    uint64_t abs_value;
    uint64_t arr_index;
    uint8_t bit_index;
    uint64_t bitmask1;
    uint8_t shift;

    arr_index = this->integer_bits * index / 64;
    bit_index = 63 - ((this->integer_bits * index) % 64);

    if (bit_index + 1 >= this->integer_bits) 
    { // Integer is not splitted
        bitmask1 = BIT_MASK(uint64_t, this->integer_bits);
        shift = bit_index + 1 - this->integer_bits;
        abs_value = (this->arr[arr_index] >> shift) & bitmask1;
    }
    else 
    { // Integer is splitted
        bitmask1 = BIT_MASK(uint64_t, bit_index + 1);
        shift = this->integer_bits - bit_index - 1;
        abs_value = (this->arr[arr_index] & bitmask1) << shift;
        abs_value = abs_value | (this->arr[arr_index + 1] >> (64 - shift));
    }

    // If negative numbers are allowed --> check sign bit
    if (!this->negative_integers)
        return abs_value;

    arr_index = index / 64;
    bit_index = 63 - (index % 64);

    if (((this->signs[arr_index] >> bit_index) & 1UL) == 1UL)
        return -(uint64_t)abs_value;

    return abs_value;
}

uint64_t *flbwt::PackedArray::get_raw_arr_pointer()
{
    return this->arr;
}

uint64_t *flbwt::PackedArray::get_raw_signs_pointer()
{
    return this->signs;
}

flbwt::PackedArray::~PackedArray()
{
    free(this->arr);
    if (this->signs != NULL)
        free(this->signs);
}