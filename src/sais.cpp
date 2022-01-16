#include <iostream>
#include "sais.hpp"
#include "utility.hpp"

// get value from B/C-vector at some index
static int64_t BC_get_value(sdsl::bit_vector *BC, uint64_t BC_base, int64_t index, uint8_t BC_width)
{
    return BC->get_int(BC_base + index * BC_width, BC_width);
}

// set value in B/C-vector at some index
static void BC_set_value(sdsl::bit_vector *BC, uint64_t BC_base, int64_t index, uint8_t BC_width, uint64_t value)
{
    BC->set_int(BC_base + index * BC_width, value, BC_width);
}

// get value from T-vector at index + 1
static uint64_t T_get_value(sdsl::bit_vector *T, uint64_t T_base, int64_t index, uint8_t T_width)
{
    return T->get_int(T_base + (index + 1) * T_width, T_width);
}

// get value from SA at some index
static int64_t SA_get_value(sdsl::bit_vector *SA, sdsl::bit_vector *SA_signs, uint64_t index, uint8_t SA_width)
{
    int64_t value = SA->get_int(index * SA_width, SA_width);
    if ((*SA_signs)[index] == 1) // sign bit set --> negative value
        value = -value;
    return value;
}

// set value in SA-vector at some index
static void SA_set_value(sdsl::bit_vector *SA, sdsl::bit_vector *SA_signs, uint64_t index, uint8_t SA_width, int64_t value)
{
    if (value < 0)
    {
        (*SA_signs)[index] = 1; // set negative sign bit
        value = -value;
    }
    else
    {
        (*SA_signs)[index] = 0;
    }

    SA->set_int(index * SA_width, value, SA_width);
}

// get the total count of each alphabet character in the input string
static void get_counts(sdsl::bit_vector *T, uint64_t T_base, uint8_t T_width, sdsl::bit_vector *C, uint64_t C_base,
                       uint8_t C_width, uint64_t n, uint64_t k)
{
    for (uint64_t i = 0; i < k; i++)
    {
        BC_set_value(C, C_base, i, C_width, 0);
    }
    for (uint64_t i = 0; i < n; i++)
    {
        uint64_t idx = T_get_value(T, T_base, i, T_width);
        uint64_t old_count = BC_get_value(C, C_base, idx, C_width);
        BC_set_value(C, C_base, idx, C_width, old_count + 1);
    }
}

// find the start or end of each bucket
static void get_buckets(sdsl::bit_vector *C, uint64_t C_base, sdsl::bit_vector *B, uint64_t B_base, uint64_t k, bool end, uint8_t BC_width)
{
    uint64_t sum = 0;

    if (end)
    {
        for (uint64_t i = 0; i < k; i++)
        {
            sum += BC_get_value(C, C_base, i, BC_width);
            BC_set_value(B, B_base, i, BC_width, sum);
        }
    }
    else
    {
        for (uint64_t i = 0; i < k; i++)
        {
            uint64_t val = BC_get_value(C, C_base, i, BC_width);
            sum += val;
            BC_set_value(B, B_base, i, BC_width, sum - val);
        }
    }
}

// compute SA
static void induce_SA(sdsl::bit_vector *T, uint64_t T_base, sdsl::bit_vector *SA, sdsl::bit_vector *SA_signs, sdsl::bit_vector *C,
                      uint64_t C_base, sdsl::bit_vector *B, uint64_t B_base, uint64_t n, uint64_t k, uint8_t T_width,
                      uint8_t BC_width, uint8_t SA_width)
{
    int64_t j;
    int64_t c0;
    int64_t c1;
    int64_t b;

    // compute SA1
    if (C_base == B_base)
    {
        get_counts(T, T_base, T_width, C, C_base, BC_width, n, k);
    }

    get_buckets(C, C_base, B, B_base, k, false, BC_width);

    j = n - 1;
    c1 = T_get_value(T, T_base, j, T_width);
    b = BC_get_value(B, B_base, c1, BC_width);

    int64_t value = ((0 < j) && ((int64_t)T_get_value(T, T_base, j - 1, T_width) < c1)) ? ~j : j;
    SA_set_value(SA, SA_signs, b, SA_width, value);
    b++;

    for (uint64_t i = 0; i < n; i++)
    {
        j = SA_get_value(SA, SA_signs, i, SA_width);
        SA_set_value(SA, SA_signs, i, SA_width, ~j);

        if (0 < j)
        {
            j--;
            c0 = T_get_value(T, T_base, j, T_width);
            if (c0 != c1)
            {
                BC_set_value(B, B_base, c1, BC_width, b);
                c1 = c0;
                b = BC_get_value(B, B_base, c1, BC_width);
            }

            value = ((0 < j) && ((int64_t)T_get_value(T, T_base, j - 1, T_width) < c1)) ? ~j : j;
            SA_set_value(SA, SA_signs, b, SA_width, value);
            b++;
        }
    }

    // compute the SAs
    if (C_base == B_base)
    {
        get_counts(T, T_base, T_width, C, C_base, BC_width, n, k);
    }

    get_buckets(C, C_base, B, B_base, k, true, BC_width);

    c1 = 0;
    b = BC_get_value(B, B_base, c1, BC_width);
    for (int64_t i = n - 1; 0 <= i; i--)
    {
        j = SA_get_value(SA, SA_signs, i, SA_width);

        if (0 < j)
        {
            j--;
            c0 = T_get_value(T, T_base, j, T_width);
            if (c0 != c1)
            {
                BC_set_value(B, B_base, c1, BC_width, b);
                c1 = c0;
                b = BC_get_value(B, B_base, c1, BC_width);
            }

            value = ((j == 0) || ((int64_t)T_get_value(T, T_base, j - 1, T_width) > c1)) ? ~j : j;
            b--;
            SA_set_value(SA, SA_signs, b, SA_width, value);
        }
        else
        {
            SA_set_value(SA, SA_signs, i, SA_width, ~j);
        }
    }
}

void flbwt::sais_main(sdsl::bit_vector *T, uint64_t T_base, sdsl::bit_vector *SA, sdsl::bit_vector *SA_signs,
                      uint64_t fs, uint64_t n, uint64_t k, uint8_t T_width, uint8_t SA_width)
{
    sdsl::bit_vector *C;  // pointer to C-vector
    sdsl::bit_vector *B;  // pointer to B-vector
    sdsl::bit_vector *BC; // pointer to BC-vector
    uint64_t C_base;      // start offset of C
    uint64_t B_base;      // start offset of B
    uint8_t BC_width;     // width of the numbers in C and B vectors
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
    int64_t RA;

    // STAGE 1: reduce the problem by at least 1/2 sort all the S-substrings
    if (k <= fs)
    {
        C = B = SA;
        C_base = n * SA_width;
        B_base = (k <= (fs - k)) ? C_base + k * SA_width : C_base;
        BC_width = SA_width;
    }
    else
    {
        BC_width = flbwt::position_of_msb(n);
        BC = new sdsl::bit_vector(k * BC_width, 0);
        C = B = BC;
        C_base = B_base = 0;
    }

    get_counts(T, T_base, T_width, C, C_base, BC_width, n, k);
    get_buckets(C, C_base, B, B_base, k, true, BC_width);

    for (uint64_t i = 0; i < n; i++)
        SA_set_value(SA, SA_signs, i, SA_width, 0);

    c = 0;
    c1 = T_get_value(T, T_base, n - 1, T_width);
    for (int64_t i = n - 2; 0 <= i; i--)
    {
        if ((c0 = T_get_value(T, T_base, i, T_width)) < (c1 + c))
        {
            c = 1;
        }
        else if (c != 0)
        {
            uint64_t index = BC_get_value(B, B_base, c1, BC_width) - 1;
            BC_set_value(B, B_base, c1, BC_width, index);
            SA_set_value(SA, SA_signs, index, SA_width, i + 1);
            c = 0;
        }

        c1 = c0;
    }

    induce_SA(T, T_base, SA, SA_signs, C, C_base, B, B_base, n, k, T_width, BC_width, SA_width);

    if (fs < k)
    {
        delete BC;
    }

    // compact all the sorted substrings into the first m items of SA
    m = 0;
    for (uint64_t i = 0; i < n; i++)
    {
        p = SA_get_value(SA, SA_signs, i, SA_width);

        if (0 < p)
        {
            int64_t prev = T_get_value(T, T_base, p - 1, T_width);
            c0 = T_get_value(T, T_base, p, T_width);

            if (prev > c0)
            {
                for (j = p + 1; (j < (int64_t)n); j++)
                {
                    c1 = T_get_value(T, T_base, j, T_width);
                    if (c0 != c1)
                        break;
                }

                if ((j < (int64_t)n) && (c0 < c1))
                {
                    SA_set_value(SA, SA_signs, m, SA_width, p);
                    m++;
                }
            }
        }
    }
    
    // int the name array buffer
    for (uint64_t i = m; i < n; i++)
        SA_set_value(SA, SA_signs, i, SA_width, 0);

    // store the length of all substrings
    j = n;
    c = 0;
    c1 = T_get_value(T, T_base, n - 1, T_width);
    for (int64_t i = n - 2; i >= 0; i--, c1 = c0)
    {
        c0 = T_get_value(T, T_base, i, T_width);

        if (c0 < c1 + c)
        {
            c = 1;
        }
        else if (c != 0)
        {
            int64_t index = m + ((i + 1) >> 1);
            SA_set_value(SA, SA_signs, index, SA_width, j - i - 1);
            j = i + 1;
            c = 0;
        }
    }

    // find the lexicographic names of all substrings
    name = 0;
    q = n;
    qlen = 0;
    for (int64_t i = 0; i < m; i++)
    {
        p = SA_get_value(SA, SA_signs, i, SA_width);
        int64_t index = m + (p >> 1);
        plen = SA_get_value(SA, SA_signs, index, SA_width);
        diff = 1;

        if (plen == qlen)
        {
            for (j = 0; j < plen; j++)
            {
                uint64_t pch = T_get_value(T, T_base, p + j, T_width);
                uint64_t qch = T_get_value(T, T_base, q + j, T_width);
                if (pch != qch)
                    break;
            }

            if (j == plen)
                diff = 0;
        }

        if (diff != 0)
        {
            name++;
            q = p;
            qlen = plen;
        }

        SA_set_value(SA, SA_signs, index, SA_width, name);
    }

    // STAGE 2: Solve the reduced problem. Recurse if names are not yet unique
    if (name < m)
    {
        RA = n + fs - m;
        for (int64_t i = n - 1, j = m - 1; m <= i; i--)
        {
            int64_t value = SA_get_value(SA, SA_signs, i, SA_width);

            if (value != 0)
            {
                SA_set_value(SA, SA_signs, RA + j, SA_width, value - 1);
                j--;
            }
        }

        flbwt::sais_main(SA, (RA - 1) * SA_width, SA, SA_signs, fs + n - m * 2, m, name, SA_width, SA_width);

        for (int64_t i = n - 2, j = m - 1, c = 0, c1 = T_get_value(T, T_base, n - 1, T_width); 0 <= i; i--, c1 = c0)
        {
            c0 = T_get_value(T, T_base, i, T_width);
            if (c0 < c1 + c)
            {
                c = 1;
            }
            else if (c != 0)
            {
                SA_set_value(SA, SA_signs, RA + j, SA_width, i + 1);
                j--;
                c = 0;
            }
        }

        for (int64_t i = 0; i < m; i++)
        {
            int64_t index = SA_get_value(SA, SA_signs, i, SA_width);
            int64_t value = SA_get_value(SA, SA_signs, RA + index, SA_width);
            SA_set_value(SA, SA_signs, i, SA_width, value);
        }
    }

    // STAGE 3: Induce the result for the original probelm
    if (k <= fs)
    {
        C = B = SA;
        C_base = n * SA_width;
        B_base = (k <= (fs - k)) ? C_base + k * SA_width : C_base;
        BC_width = SA_width;
    }
    else
    {
        BC_width = flbwt::position_of_msb(n);
        BC = new sdsl::bit_vector(k * BC_width, 0);
        C = B = BC;
        C_base = B_base = 0;
    }

    // put all LMS characters into their buckets
    get_counts(T, T_base, T_width, C, C_base, BC_width, n, k);
    get_buckets(C, C_base, B, B_base, k, true, BC_width);

    for (uint64_t i = m; i < n; i++)
       SA_set_value(SA, SA_signs, i, SA_width, 0);

    for (int64_t i = m - 1; 0 <= i; i--)
    {
       j = SA_get_value(SA, SA_signs, i, SA_width);
       SA_set_value(SA, SA_signs, i, SA_width, 0);

       int64_t index = T_get_value(T, T_base, j, T_width);
       int64_t value = BC_get_value(B, B_base, index, BC_width) - 1;
       BC_set_value(B, B_base, index, BC_width, value);
       SA_set_value(SA, SA_signs, value, SA_width, j);
    }

    induce_SA(T, T_base, SA, SA_signs, C, C_base, B, B_base, n, k, T_width, BC_width, SA_width);

    if (fs < k)
    {
        delete BC;
    }
}