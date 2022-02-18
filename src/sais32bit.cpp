#include <iostream>
#include <stddef.h>
#include "sais32bit.hpp"
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


#define chr(i) ((cs == 32) ? (((const uint32_t *)T)[i]) : (get2((uint64_t *)T, i + 1, cs)))

/**
 * @brief Get the count of each character in the input string.
 */
static void get_counts(const uint8_t *T, int32_t *C, uint64_t n, uint64_t k, uint8_t cs)
{
    for (uint64_t i = 0; i < k; ++i)
        C[i] = 0;

    for (uint64_t i = 0; i < n; ++i)
        ++C[chr(i)];
}

/**
 * @brief Find the start or end of each bucket.
 */
static void get_buckets(const int32_t *C, int32_t *B, uint64_t k, bool end)
{
    uint64_t sum = 0;

    if (end)
    {
        for (uint64_t i = 0; i < k; ++i)
        {
            sum += C[i];
            B[i] = sum;
        }
    }
    else
    {
        for (uint64_t i = 0; i < k; ++i)
        {
            sum += C[i];
            B[i] = sum - C[i];
        }
    }
}

/**
 * @brief Induce SA.
 */
static void induce_SA(const uint8_t *T, int32_t *SA, int32_t *C, int32_t *B, uint64_t n, uint64_t k, uint8_t cs)
{
    int32_t j;
    int32_t c0;
    int32_t c1;
    int32_t *b;

    // compute SA1
    if (C == B)
    {
        get_counts(T, C, n, k, cs);
    }

    get_buckets(C, B, k, false);

    j = n - 1;
    c1 = chr(j);
    b = SA + B[c1];

    *b++ = ((0 < j) && (chr(j - 1) < (uint32_t)c1)) ? ~j : j;

    for (uint64_t i = 0; i < n; ++i)
    {
        j = SA[i];
        SA[i] = ~j;

        if (0 < j)
        {
            --j;
            c0 = chr(j);
            if (c0 != c1)
            {
                B[c1] = b - SA;
                c1 = c0;
                b = SA + B[c1];
            }

            *b++ = ((0 < j) && (chr(j - 1) < (uint32_t)c1)) ? ~j : j;
        }
    }

    // compute the SAs
    if (C == B)
    {
        get_counts(T, C, n, k, cs);
    }

    get_buckets(C, B, k, true);

    c1 = 0;
    b = SA + B[c1];
    for (int64_t i = n - 1; 0 <= i; --i)
    {
        j = SA[i];

        if (0 < j)
        {
            --j;
            c0 = chr(j);
            if (c0 != c1)
            {
                B[c1] = b - SA;
                c1 = c0;
                b = SA + B[c1];
            }

            *--b = ((j == 0) || (chr(j - 1) > (uint32_t)c1)) ? ~j : j;
        }
        else
        {
            SA[i] = ~j;
        }
    }
}

void flbwt::sais_32bit(const uint8_t *T, int32_t *SA, uint64_t fs, uint64_t n, uint64_t k, uint8_t cs)
{
    int32_t *C = NULL;
    int32_t *B = NULL;
    int32_t *RA = NULL;
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
        C = SA + n;
        B = (k <= (fs - k)) ? C + k : C;
    }
    else
    {
        C = B = new int32_t[k];
    }

    get_counts(T, C, n, k, cs);
    get_buckets(C, B, k, true);

    for (uint64_t i = 0; i < n; ++i)
        SA[i] = 0;

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
            SA[--B[c1]] = i + 1;
            c = 0;
        }

        c1 = c0;
    }

    induce_SA(T, SA, C, B, n, k, cs);

    if (fs < k)
        delete[] C;    

    // compact all the sorted substrings into the first m items of SA
    m = 0;
    for (uint64_t i = 0; i < n; ++i)
    {
        p = SA[i];

        if (0 < p)
        {
            int32_t prev = chr(p - 1);
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
                    SA[m++] = p;
            }
        }
    }

    // int the name array buffer
    for (uint64_t i = m; i < n; ++i)
        SA[i] = 0;

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
            SA[m + ((i + 1) >> 1)] = j - i - 1;
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
        p = SA[i];
        int64_t index = m + (p >> 1);
        plen = SA[index];
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

        SA[index] = name;
    }

    // STAGE 2: Solve the reduced problem. Recurse if names are not yet unique
    if (name < m)
    {
        RA = SA + n + fs - m;
        for (int64_t i = n - 1, j = m - 1; m <= i; --i)
        {
            if (SA[i] != 0)
                RA[j--] = SA[i] - 1;
        }

        flbwt::sais_32bit((uint8_t *)RA, SA, fs + n - m * 2, m, name, 32);

        for (int64_t i = n - 2, j = m - 1, c = 0, c1 = chr(n - 1); 0 <= i; --i, c1 = c0)
        {
            c0 = chr(i);
            if (c0 < c1 + c)
            {
                c = 1;
            }
            else if (c != 0)
            {
                RA[j--] = i + 1;
                c = 0;
            }
        }

        for (int64_t i = 0; i < m; ++i)
            SA[i] = RA[SA[i]];
    }

    // STAGE 3: Induce the result for the original probelm
    if (k <= fs)
    {
        C = SA + n;
        B = (k <= (fs - k)) ? C + k : C;
    }
    else
    {
        C = B = new int32_t[k];
    }

    // put all LMS characters into their buckets
    get_counts(T, C, n, k, cs);
    get_buckets(C, B, k, true);

    for (uint64_t i = m; i < n; ++i)
        SA[i]= 0;

    for (int64_t i = m - 1; 0 <= i; --i)
    {
        j = SA[i];
        SA[i] = 0;
        SA[--B[chr(j)]] = j;
    }

    induce_SA(T, SA, C, B, n, k, cs);

    if (fs < k)
        delete[] C;
}