#include <cstdlib>
#include <stddef.h>
#include "packed_array.hpp"

flbwt::PackedArray::PackedArray(uint64_t length, uint64_t max_integer, bool support_negative_integers)
{
    this->length = length;
    this->negative_integers = support_negative_integers;

    // Determine bits for single integer (and allocate arrays)
    this->arr = (uint8_t **)malloc(8 * sizeof(uint8_t *));
    uint8_t index = 0;
    this->integer_bits = 0;
    this->allocated_arrays = 0;
    while (max_integer != 0)
    {
        this->integer_bits += 8;
        this->arr[index++] = (uint8_t *)malloc(length * sizeof(uint8_t));
        ++this->allocated_arrays;
        max_integer >>= 8;
    }

    // If negative numbers should be supported, allocate space for sign bits
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

    for (int64_t i = signs_length; i--;)
        this->signs[i] = 0;
}

flbwt::PackedArray::PackedArray(uint64_t length, bool support_negative_integers, uint8_t integer_bits)
{
    this->length = length;
    this->negative_integers = support_negative_integers;

    // Determine bits for single integer (and allocate arrays)
    this->arr = (uint8_t **)malloc(8 * sizeof(uint8_t *));
    uint8_t index = 0;
    this->integer_bits = integer_bits;
    this->allocated_arrays = 0;
    while (integer_bits != 0)
    {
        this->arr[index++] = (uint8_t *)malloc(length * sizeof(uint8_t));
        ++this->allocated_arrays;
        if (integer_bits > 8)
        {
            integer_bits -= 8;
        }
        else
        {
            break;
        }
    }

    // If negative numbers should be supported, allocate space for sign bits
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

    for (int64_t i = signs_length; i--;)
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

void flbwt::PackedArray::set_value(uint64_t index, int64_t value)
{
    uint64_t abs_value;

    // Set the sign bit first, if supported
    if (this->negative_integers)
    {
        uint64_t arr_index = index / 64;
        uint8_t bit_index = 63 - (index % 64);

        if (value < 0)
        { // Negative value --> set sign bit
            abs_value = -value;
            this->signs[arr_index] |= (1UL << bit_index);
        }
        else
        { // Positive value --> clear sign bit
            abs_value = value;
            this->signs[arr_index] &= ~(1UL << bit_index);
        }
    }
    else
    {
        abs_value = value;
    }

    // Set the absolute value
    for(int8_t i = 0; i < this->allocated_arrays; i++)
    {
        this->arr[i][index] = abs_value;
        abs_value >>= 8;
    }
}

int64_t flbwt::PackedArray::get_value(uint64_t index)
{
    int64_t value = 0;

    // Get the absolute value
    for(int8_t i = this->allocated_arrays - 1; i >= 0; i--)
        value = (value << 8) | this->arr[i][index];

    // Get negative sign bit --> if supported
    if (!this->negative_integers)
        return value;

    uint64_t arr_index = index / 64;
    uint8_t bit_index = 63 - (index % 64);

    if (((this->signs[arr_index] >> bit_index) & 1UL) == 1UL)
        return -value;

    return value;
}

uint8_t **flbwt::PackedArray::get_raw_arr_pointer()
{
    return this->arr;
}

uint64_t *flbwt::PackedArray::get_raw_signs_pointer()
{
    return this->signs;
}

flbwt::PackedArray::~PackedArray()
{
    for (int8_t i = 0; i < this->allocated_arrays; i++)
        free(this->arr[i]);
    free(this->arr);
    
    if (this->signs != NULL)
        free(this->signs);
}