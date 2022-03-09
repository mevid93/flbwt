#include <iostream>
#include <stddef.h>
#include "sais40bit.hpp"
#include "utility.hpp"

/**
 * @brief Get value from packed array.
 */
static uint64_t get2(uint64_t *B, uint64_t i, uint8_t d)
{
    uint64_t abs_value;
    uint64_t arr_index;
    uint8_t bit_index;
    uint64_t bitmask1;
    uint8_t shift;

    arr_index = d * i / 64;
    bit_index = 63 - ((d * i) % 64);

    if (bit_index + 1 >= d)
    { // Integer is not splitted
        bitmask1 = BIT_MASK(uint64_t, d);
        shift = bit_index + 1 - d;
        abs_value = (B[arr_index] >> shift) & bitmask1;
    }
    else
    { // Integer is splitted
        bitmask1 = BIT_MASK(uint64_t, bit_index + 1);
        shift = d - bit_index - 1;
        abs_value = (B[arr_index] & bitmask1) << shift;
        abs_value = abs_value | (B[arr_index + 1] >> (64 - shift));
    }

    return abs_value;
}

#define chr(i) ((cs == 40) ? ((int64_t)TA_U[i] << 32 | TA_L[i]) : (get2((uint64_t *)T, i + 1, cs)))

/**
 * @brief Get the count of each character in the input string.
 */
static void get_counts(const uint8_t *T, const uint32_t *TA_L, const int8_t *TA_U,
                       uint32_t *C_L, int8_t *C_U, uint64_t n, uint64_t k, uint8_t cs)
{
    for (uint64_t i = 0; i < k; ++i)
        C_L[i] = C_U[i] = 0;

    for (uint64_t i = 0; i < n; ++i)
    {
        uint64_t index = chr(i);
        uint64_t value = ((uint64_t)C_U[index] << 32 | C_L[index]) + 1;
        C_U[index] = value >> 32;
        C_L[index] = value & 0xffffffff;
    }
}

/**
 * @brief Find the start or end of each bucket.
 */
static void get_buckets(const uint32_t *C_L, const int8_t *C_U, uint32_t *B_L, int8_t *B_U, uint64_t k, bool end)
{
    uint64_t sum = 0;

    if (end)
    {
        for (uint64_t i = 0; i < k; ++i)
        {
            sum += ((uint64_t)C_U[i] << 32 | C_L[i]);
            B_U[i] = sum >> 32;
            B_L[i] = sum & 0xffffffff;
        }
    }
    else
    {
        uint64_t value;
        uint64_t sub;
        for (uint64_t i = 0; i < k; ++i)
        {
            value = ((uint64_t)C_U[i] << 32 | C_L[i]);
            sub = sum;
            sum += value;
            B_U[i] = sub >> 32;
            B_L[i] = sub & 0xffffffff;
        }
    }
}

/**
 * @brief Induce SA.
 */
static void induce_SA(const uint8_t *T, const uint32_t *TA_L, const int8_t *TA_U, uint32_t *SA_L, int8_t *SA_U,
                      uint32_t *C_L, int8_t *C_U, uint32_t *B_L, int8_t *B_U, uint64_t n, uint64_t k, uint8_t cs)
{
    int64_t j;
    int64_t c0;
    int64_t c1;
    uint32_t *b_L;
    int8_t *b_U;

    // compute SA1
    if (C_L == B_L)
    {
        get_counts(T, TA_L, TA_U, C_L, C_U, n, k, cs);
    }

    get_buckets(C_L, C_U, B_L, B_U, k, false);

    j = n - 1;
    c1 = chr(j);
    uint64_t value = flbwt::get_40bit_value(B_L, B_U, c1);
    b_U = SA_U + value; 
    b_L = SA_L + value;

    int64_t result = ((0 < j) && (chr(j - 1) < (uint64_t)c1)) ? ~j : j;
    flbwt::set_40bit_value(b_L++, b_U++, 0, result);

    for (uint64_t i = 0; i < n; ++i)
    {
        j = flbwt::get_40bit_value(SA_L, SA_U, i);
        flbwt::set_40bit_value(SA_L, SA_U, i, ~j);

        if (0 < j)
        {
            --j;

            c0 = chr(j);
            if (c0 != c1)
            {
                flbwt::set_40bit_value(B_L, B_U, c1, b_L - SA_L);
                c1 = c0;
                value = flbwt::get_40bit_value(B_L, B_U, c1);
                b_U = SA_U + value; 
                b_L = SA_L + value;
            }

            result = ((0 < j) && (chr(j - 1) < (uint64_t)c1)) ? ~j : j;
            flbwt::set_40bit_value(b_L++, b_U++, 0, result);
        }
    }

    // compute the SAs
    if (C_L == B_L)
    {
        get_counts(T, TA_L, TA_U, C_L, C_U, n, k, cs);
    }

    get_buckets(C_L, C_U, B_L, B_U, k, true);

    c1 = 0;
    value = flbwt::get_40bit_value(B_L, B_U, c1);
    b_U = SA_U + value; 
    b_L = SA_L + value;
    for (int64_t i = n - 1; 0 <= i; --i)
    {
        j = flbwt::get_40bit_value(SA_L, SA_U, i);

        if (0 < j)
        {
            --j;
            c0 = chr(j);
            if (c0 != c1)
            {
                flbwt::set_40bit_value(B_L, B_U, c1, b_L - SA_L);
                c1 = c0;
                value = flbwt::get_40bit_value(B_L, B_U, c1);
                b_U = SA_U + value; 
                b_L = SA_L + value;
            }

            result = ((j == 0) || (chr(j - 1) > (uint64_t)c1)) ? ~j : j;
            --b_L;
            --b_U;
            flbwt::set_40bit_value(b_L, b_U, 0, result); 
        }
        else
        {
            flbwt::set_40bit_value(SA_L, SA_U, i, ~j);
        }
    }
}

void flbwt::sais_40bit(const uint8_t *T, uint32_t *TA_L, int8_t *TA_U, uint32_t *SA_L,
                       int8_t *SA_U, uint64_t fs, uint64_t n, uint64_t k, uint8_t cs)
{
    uint32_t *C_L = NULL;
    uint32_t *B_L = NULL;
    uint32_t *RA_L = NULL;
    int8_t *C_U = NULL;
    int8_t *B_U = NULL;
    int8_t *RA_U = NULL;
    int64_t c;
    int64_t c0;
    int64_t c1;
    int64_t m;
    int64_t p;
    int64_t j;
    int64_t name;
    int64_t q;
    int64_t qlen;
    int64_t plen;
    int64_t diff;

    // STAGE 1: reduce the problem by at least 1/2 sort all the S-substrings
    if (k <= fs)
    {
        C_L = SA_L + n;
        C_U = SA_U + n;
        B_L = (k <= (fs - k)) ? C_L + k : C_L;
        B_U = (k <= (fs - k)) ? C_U + k : C_U;
    }
    else
    {
        C_L = B_L = new uint32_t[k];
        C_U = B_U = new int8_t[k];
        flbwt::increase_memory_allocation(5*k);
    }

    get_counts(T, TA_L, TA_U, C_L, C_U, n, k, cs);
    get_buckets(C_L, C_U, B_L, B_U, k, true);

    for (uint64_t i = 0; i < n; ++i)
        SA_L[i] = SA_U[i] = 0;

    c = 0;
    c1 = chr(n - 1);
    for (int64_t i = n - 2; 0 <= i; --i)
    {
        if ((c0 = chr(i)) < (c1 + c))
        {
            c = 1;
        }
        else if (c != 0)
        {
            int64_t index = flbwt::get_40bit_value(B_L, B_U, c1) - 1;
            flbwt::set_40bit_value(B_L, B_U, c1, index);
            flbwt::set_40bit_value(SA_L, SA_U, index, i + 1);
            c = 0;
        }

        c1 = c0;
    }

    induce_SA(T, TA_L, TA_U, SA_L, SA_U, C_L, C_U, B_L, B_U, n, k, cs);

    if (fs < k)
    {
        delete[] C_L;
        delete[] C_U;
        flbwt::decrease_memory_allocation(5*k);
    }
    
    // compact all the sorted substrings into the first m items of SA
    m = 0;
    for (uint64_t i = 0; i < n; ++i)
    {
        p = flbwt::get_40bit_value(SA_L, SA_U, i);

        if (0 < p)
        {
            int64_t prev = chr(p - 1);
            c0 = chr(p);

            if (prev > c0)
            {
                for (j = p + 1; (j < (int64_t)n); ++j)
                {
                    c1 = chr(j);
                    if (c0 != c1)
                        break;
                }

                if ((j < (int64_t)n) && (c0 < c1))
                    flbwt::set_40bit_value(SA_L, SA_U, m++, p);
            }
        }
    }

    // // int the name array buffer
    for (uint64_t i = m; i < n; ++i)
        SA_L[i] = SA_U[i] = 0;

    // store the length of all substrings
    j = n;
    c = 0;
    c1 = chr(n - 1);
    for (int64_t i = n - 2; i >= 0; --i, c1 = c0)
    {
        c0 = chr(i);

        if (c0 < c1 + c)
        {
            c = 1;
        }
        else if (c != 0)
        {
            flbwt::set_40bit_value(SA_L, SA_U, m + ((i + 1) >> 1), j - i - 1);
            j = i + 1;
            c = 0;
        }
    }

    // find the lexicographic names of all substrings
    name = 0;
    q = n;
    qlen = 0;
    for (int64_t i = 0; i < m; ++i)
    {
        p = flbwt::get_40bit_value(SA_L, SA_U, i);
        int64_t index = m + (p >> 1);
        plen = flbwt::get_40bit_value(SA_L, SA_U, index);
        diff = 1;

        if (plen == qlen)
        {
            for (j = 0; j < plen; ++j)
            {
                if (chr(p + j) != chr(q + j))
                    break;
            }

            if (j == plen)
                diff = 0;
        }

        if (diff != 0)
        {
            ++name;
            q = p;
            qlen = plen;
        }

        flbwt::set_40bit_value(SA_L, SA_U, index, name);
    }

    // STAGE 2: Solve the reduced problem. Recurse if names are not yet unique
    if (name < m)
    {
        RA_L = SA_L + n + fs - m;
        RA_U = SA_U + n + fs - m;
        for (int64_t i = n - 1, j = m - 1; m <= i; --i)
        {
            if (SA_L[i] != 0 || SA_U[i] != 0)
            {
                int64_t value = flbwt::get_40bit_value(SA_L, SA_U, i) - 1;
                flbwt::set_40bit_value(RA_L, RA_U, j--, value);
            }
        }

        flbwt::sais_40bit(NULL, RA_L, RA_U, SA_L, SA_U, fs + n - m * 2, m, name, 40);

        for (int64_t i = n - 2, j = m - 1, c = 0, c1 = chr(n - 1); 0 <= i; --i, c1 = c0)
        {
            c0 = chr(i);

            if (c0 < c1 + c)
            {
                c = 1;
            }
            else if (c != 0)
            {
                flbwt::set_40bit_value(RA_L, RA_U, j--, i + 1);
                c = 0;
            }
        }

        for (int64_t i = 0; i < m; ++i)
        {
            int64_t index = flbwt::get_40bit_value(SA_L, SA_U, i);
            int64_t value = flbwt::get_40bit_value(RA_L, RA_U, index);
            flbwt::set_40bit_value(SA_L, SA_U, i, value);
        }
    }

    // STAGE 3: Induce the result for the original probelm
    if (k <= fs)
    {
        C_L = SA_L + n;
        C_U = SA_U + n;
        B_L = (k <= (fs - k)) ? C_L + k : C_L;
        B_U = (k <= (fs - k)) ? C_U + k : C_U;
    }
    else
    {
        C_L = B_L = new uint32_t[k];
        C_U = B_U = new int8_t[k];
        flbwt::increase_memory_allocation(k*5);
    }

    // put all LMS characters into their buckets
    get_counts(T, TA_L, TA_U, C_L, C_U, n, k, cs);
    get_buckets(C_L, C_U, B_L, B_U, k, true);

    for (uint64_t i = m; i < n; ++i)
        SA_L[i] = SA_U[i] = 0;

    for (int64_t i = m - 1; 0 <= i; --i)
    {
        j = flbwt::get_40bit_value(SA_L, SA_U, i);
        SA_L[i] = SA_U[i] = 0;
        int64_t index1 = chr(j);
        uint64_t index2 = flbwt::get_40bit_value(B_L, B_U, index1) - 1;
        flbwt::set_40bit_value(B_L, B_U, index1, index2);
        flbwt::set_40bit_value(SA_L, SA_U, index2, j);
    }

    induce_SA(T, TA_L, TA_U, SA_L, SA_U, C_L, C_U, B_L, B_U, n, k, cs);

    if (fs < k)
    {
        delete[] C_L;
        delete[] C_U;
        flbwt::decrease_memory_allocation(k*5);
    }
}