#include <iostream>
#include <stddef.h>
#include "packed_array.hpp"

flbwt::PackedArray::PackedArray(uint64_t length, uint64_t max_integer, bool support_negative_integers)
{
    this->length = length;
    this->max_integer = max_integer;
    this->negative_integers = support_negative_integers;

    // determine bits for single integer (and allocate arrays)
    this->arr = (uint8_t**)malloc(8 * sizeof(uint8_t*));
    uint8_t index = 0;
    this->integer_bits = 0;
    this->allocated_arrays = 0;
    while(max_integer != 0)
    {
        this->integer_bits += 8;
        this->arr[index++] = (uint8_t *)malloc(length * sizeof(uint8_t));
        ++this->allocated_arrays;
        max_integer >>= 8;
    }

    // if negative numbers should be supported, allocate space for sign bits
    if (!this->negative_integers)
    {
        this->signs = NULL;
        this->signs_length = 0;
        return;
    }

    uint64_t bits_required = length;
    uint64_t signs_length = bits_required / 64 + 1;
    this->signs = (uint64_t *)malloc(signs_length * sizeof(uint64_t));
    this->signs_length = signs_length;

    for(int64_t i = signs_length; i--;)
        this->signs[i] = 0;
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
        uint8_t bit_index = 63 - (index % 64);

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

    // set the absolute value
    int8_t bits = this->integer_bits;
    uint8_t arr_idx = 0;
    while(bits != 0)
    {
        this->arr[arr_idx][index] = abs_value; // assigns first 8 bits
        abs_value >>= 8;
        ++arr_idx;
        bits -= 8;
    }
}

int64_t flbwt::PackedArray::get_value(uint64_t index)
{
    int8_t array_idx = this->integer_bits / 8 - 1;
    int64_t value = 0;
    while(array_idx != -1)
    {
        value = (value << 8) | this->arr[array_idx][index];
        --array_idx;
    }

    // check if negative numbers are allowed
    // if allowed --> need to check sign bit
    if (!this->negative_integers)
        return value;

    uint64_t arr_index = index / 64;
    uint8_t bit_index = 63 - (index % 64);

    // check if sign bit is set --> negative integer
    if (((this->signs[arr_index] >> bit_index) & 1UL) == 1UL)
        return -value;

    return value; // sign bit was not set --> positive integer
}

uint8_t **flbwt::PackedArray::get_raw_arr_pointer()
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
    for (int8_t i = 0; i < this->allocated_arrays; i++)
        this->arr[i] = (uint8_t *)realloc(this->arr[i], length * sizeof(uint8_t));

    if (this->signs == NULL)
        return;

    // release memory from the supporting sign array
    uint64_t bits_required = length;
    uint64_t signs_length = bits_required / 64 + 1;
    this->signs = (uint64_t *)realloc(this->signs, signs_length * sizeof(uint64_t));
    this->signs_length = signs_length;
}

flbwt::PackedArray::~PackedArray()
{
    for (int8_t i = 0; i < this->allocated_arrays; i++)
        free(this->arr[i]);
    free(this->arr);
    if (this->signs != NULL)
        free(this->signs);
}