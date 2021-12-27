#include "utility.hpp"

uint8_t flbwt::calculate_alphabet_size(const uint8_t *T, const uint64_t n)
{
    uint8_t alphabet[256] = {0};
    uint8_t size = 0;

    uint64_t i;
    for (i = 0; i < n; i++)
    {
        uint8_t val = T[i];

        if (alphabet[val] == 0)
        {
            alphabet[val]++;
            size++;
        }
    }

    return size;
}

const int tab64[64] = {
    63, 0, 58, 1, 59, 47, 53, 2,
    60, 39, 48, 27, 54, 33, 42, 3,
    61, 51, 37, 40, 49, 18, 28, 20,
    55, 30, 34, 11, 43, 14, 22, 4,
    62, 57, 46, 52, 38, 26, 32, 41,
    50, 36, 17, 19, 29, 10, 13, 21,
    56, 45, 25, 31, 35, 16, 9, 12,
    44, 24, 15, 8, 23, 7, 6, 5};

int flbwt::log2_64(uint64_t value)
{
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value |= value >> 32;
    return tab64[((uint64_t)((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >> 58];
}

uint8_t flbwt::position_of_msb(const uint64_t n)
{
    uint8_t position = 0;

    for (int i = 0; i < 64; i++)
    {
        uint8_t bset = n >> i & 1;

        if (bset == 1) {
            position = i;
        }
    }

    return position + 1;
}