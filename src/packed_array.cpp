#include <iostream>
#include <algorithm>
#include <stddef.h>
#include "packed_array.hpp"

flbwt::PackedArray::PackedArray(uint64_t length, uint64_t max_integer, bool support_negative_integers)
{
    this->length = length;
    this->max_integer = max_integer;
    this->negative_integers = support_negative_integers;

    // determine bits for single integer
    if (max_integer <= 0xff)
    {
        this->integer_bits = 8;
    }
    else if (max_integer > 0xff && max_integer <= 0xffff)
    {
        this->integer_bits = 16;
    }
    else if (max_integer > 0xffff && max_integer <= 0xffffff)
    {
        this->integer_bits = 24;
    }
    else if (max_integer > 0xffffff && max_integer <= 0xffffffff)
    {
        this->integer_bits = 32;
    }
    else if (max_integer > 0xffffffff && max_integer <= 0xffffffffff)
    {
        this->integer_bits = 40;
    }
    else if (max_integer > 0xffffffffff && max_integer <= 0xffffffffffff)
    {
        this->integer_bits = 48;
    }
    else if (max_integer > 0xffffffffffff && max_integer <= 0xffffffffffffff)
    {
        this->integer_bits = 56;
    }
    else
    {
        this->integer_bits = 64;
    }

    // allocate space for integers (excluding sign bits)
    uint64_t bits_required = length * this->integer_bits;
    uint64_t arr_length = bits_required / 64 + 1;
    this->arr = (uint64_t *)malloc(arr_length * sizeof(uint64_t));
    this->arr_length = arr_length;
    std::fill_n(this->arr, arr_length, 0);

    // if negative numbers should be supported, allocate space for sign bits
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
    std::fill_n(this->signs, signs_length, 0);
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

uint64_t flbwt::PackedArray::get_maximum_supported_integer()
{
    return this->max_integer;
}

void flbwt::PackedArray::set_value(uint64_t index, int64_t value)
{
    uint64_t abs_value;

    // set the sign bit first, if supported
    if (this->negative_integers)
    {
        uint64_t arr_index = index / 64;
        uint64_t bit_index = 63 - (index % 64);

        if (value < 0) // negative value --> set sign bit
        {
            abs_value = -value;
            this->signs[arr_index] |= (1UL << bit_index);
        }
        else // positive value --> clear sign bit
        {
            abs_value = value;
            this->signs[arr_index] &= ~(1UL << bit_index);
        }
    }
    else
    {
        abs_value = value;
    }

    // set the value (excluding sign bit)
    uint64_t arr_index = this->integer_bits * index / 64;
    uint64_t bit_index = 63 - ((this->integer_bits * index) % 64);

    // abs_value may too big for the space that is left at the arr_index. Can happen for abs_value > 2^32.
    // if this is the case, abs_value needs to be splitted and the leftover goes to next index in arr.
    // since the maximum abs_value is 64 bit integer --> every value can be fitted into two arr indices.
    uint8_t width_first_part, width_second_part;

    if (bit_index + 1 >= this->integer_bits) // there is enough space --> no need to split
    {
        width_first_part = this->integer_bits;
        width_second_part = 0;
    }
    else // not enough space --> split the bits
    {
        width_first_part = bit_index + 1;
        width_second_part = this->integer_bits - width_first_part;
    }

    uint64_t bit_mask;
    uint8_t shift;

    // handle the case where there is no need for splitting
    if (width_second_part == 0)
    {
        // first clear existing bits
        switch (this->integer_bits)
        {
        case 8:
            shift = bit_index - 8 + 1;
            bit_mask = 0xffUL << shift;
            break;
        case 16:
            shift = bit_index - 16 + 1;
            bit_mask = 0xffffUL << shift;
            break;
        case 24:
            shift = bit_index - 24 + 1;
            bit_mask = 0xffffffUL << shift;
            break;
        case 32:
            shift = bit_index - 32 + 1;
            bit_mask = 0xffffffffUL << shift;
            break;
        case 40:
            shift = bit_index - 40 + 1;
            bit_mask = 0xffffffffffUL << shift;
            break;
        case 48:
            shift = bit_index - 48 + 1;
            bit_mask = 0xffffffffffffUL << shift;
            break;
        case 56:
            shift = bit_index - 56 + 1;
            bit_mask = 0xffffffffffffffUL << shift;
            break;
        default: // 64
            shift = 0;
            bit_mask = 0xffffffffffffffffUL;
            break;
        }

        // assign new bit values
        this->arr[arr_index] = (this->arr[arr_index] & (~bit_mask)) | (abs_value << shift);
        return;
    }

    // handle the case where splitting is needed...
    // first we need to assign bits that can be fitted to remaining space
    uint64_t bits_first_part = abs_value >> (this->integer_bits - width_first_part);
    bit_mask = 0xffffffffffffffffUL >> (64 - width_first_part);
    this->arr[arr_index] = (this->arr[arr_index] & (~bit_mask)) | bits_first_part;

    // then we assign the remaining bits to next index of arr
    uint64_t bits_second_part = abs_value << (64 - width_second_part);
    bit_mask = 0xffffffffffffffffUL << (64 - width_second_part);
    this->arr[arr_index + 1] = (this->arr[arr_index + 1] & (~bit_mask)) | bits_second_part;
}

int64_t flbwt::PackedArray::get_value(uint64_t index)
{
    int64_t value;
    uint64_t arr_index;
    uint64_t bit_index;
    uint8_t width_first_part;
    uint8_t width_second_part;
    uint64_t bit_mask;
    uint8_t shift;

    arr_index = this->integer_bits * index / 64;
    bit_index = 63 - ((this->integer_bits * index) % 64);

    if (bit_index + 1 >= this->integer_bits) // integer is not splitted
    {
        width_first_part = this->integer_bits;
        width_second_part = 0;
    }
    else // integer was splitted
    {
        width_first_part = bit_index + 1;
        width_second_part = this->integer_bits - width_first_part;
    }

    // handle the case where there is was no splitting
    if (width_second_part == 0)
    {
        switch (this->integer_bits)
        {
        case 8:
            shift = bit_index - 8 + 1;
            bit_mask = 0xffUL;
            break;
        case 16:
            shift = bit_index - 16 + 1;
            bit_mask = 0xffffUL;
            break;
        case 24:
            shift = bit_index - 24 + 1;
            bit_mask = 0xffffffUL;
            break;
        case 32:
            shift = bit_index - 32 + 1;
            bit_mask = 0xffffffffUL;
            break;
        case 40:
            shift = bit_index - 40 + 1;
            bit_mask = 0xffffffffffUL;
            break;
        case 48:
            shift = bit_index - 48 + 1;
            bit_mask = 0xffffffffffffUL;
            break;
        case 56:
            shift = bit_index - 56 + 1;
            bit_mask = 0xffffffffffffffUL;
            break;
        default: // 64
            shift = 0;
            bit_mask = 0xffffffffffffffffUL;
            break;
        }

        // assign new bit values
        value = (this->arr[arr_index] >> shift) & bit_mask;
    }
    else
    {
        // handle the case where integer was splitted...
        // first we need to retrieve first part bits
        bit_mask = 0xffffffffffffffffUL >> (64 - width_first_part);
        value = (this->arr[arr_index] & bit_mask) << width_second_part;

        // then we get the remaining bits from the next index of arr
        value = value | (this->arr[arr_index + 1] >> (64 - width_second_part));
    }

    // check if negative numbers are allowed
    // if allowed --> need to check sign bit
    if (!this->negative_integers)
        return value;

    arr_index = index / 64;
    bit_index = 63 - (index % 64);

    // check if sign bit is set --> negative integer
    if (((this->signs[arr_index] >> bit_index) & 1UL) == 1UL)
        return -value;

    return value; // sign bit was not set --> positive integer
}

uint64_t *flbwt::PackedArray::get_raw_arr_pointer()
{
    return this->arr;
}

uint64_t *flbwt::PackedArray::get_raw_signs_pointer()
{
    return this->signs;
}

void flbwt::PackedArray::reallocate(uint64_t length)
{
    this->length = length;

    // release memory from the main array
    uint64_t bits_required = length * this->integer_bits;
    uint64_t arr_length = bits_required / 64 + 1;
    this->arr = (uint64_t *)realloc(this->arr, arr_length * sizeof(uint64_t));
    this->arr_length = arr_length;

    if (this->signs == NULL)
        return;

    // release memory from the supporting sign array
    bits_required = length;
    uint64_t signs_length = bits_required / 64 + 1;
    this->signs = (uint64_t *)realloc(this->signs, signs_length * sizeof(uint64_t));
    this->signs_length = signs_length;
}

flbwt::PackedArray::~PackedArray()
{
    free(this->arr);
    if (this->signs != NULL)
        free(this->signs);
}