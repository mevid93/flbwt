#include "utility.h"

uint8_t calculate_alphabet_size(const uint8_t *T, const uint64_t n)
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