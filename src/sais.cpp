#include <iostream>
#include "sais.hpp"
#include "utility.hpp"

// get the total count of each alphabet character in the input string
static void get_counts(flbwt::PackedArray *T, uint64_t T_base, flbwt::PackedArray *C, uint64_t C_base, uint64_t n, uint64_t k)
{
    for (uint64_t i = 0; i < k; i++)
    {
        C->set_value(C_base + i, 0);
    }
    for (uint64_t i = 0; i < n; i++)
    {
        uint64_t idx = T->get_value(T_base + i + 1);
        uint64_t old_count = C->get_value(C_base + idx);
        C->set_value(C_base + idx, old_count + 1);   
    }
}

// find the start or end of each bucket
static void get_buckets(flbwt::PackedArray *C, uint64_t C_base, flbwt::PackedArray *B, uint64_t B_base, uint64_t k, bool end)
{
    uint64_t sum = 0;

    if (end)
    {
        for (uint64_t i = 0; i < k; i++)
        {
            sum += C->get_value(C_base + i);
            B->set_value(B_base + i, sum);
        }
    }
    else
    {
        for (uint64_t i = 0; i < k; i++)
        {
            uint64_t val = C->get_value(C_base + i);
            sum += val;
            B->set_value(B_base + i, sum - val);
        }
    }
}

// compute SA
static void induce_SA(flbwt::PackedArray *T, uint64_t T_base, flbwt::PackedArray *SA, flbwt::PackedArray *C, uint64_t C_base,
                      flbwt::PackedArray *B, uint64_t B_base, uint64_t n, uint64_t k)
{
    int64_t j;
    int64_t c0;
    int64_t c1;
    int64_t b;

    // compute SA1
    if (C_base == B_base)
    {
        get_counts(T, T_base, C, C_base, n, k);
    }

    get_buckets(C, C_base, B, B_base, k, false);

    j = n - 1;
    c1 = T->get_value(T_base + j + 1);
    b = B->get_value(B_base + c1);

    int64_t value = ((0 < j) && (T->get_value(T_base + j) < c1)) ? ~j : j;
    SA->set_value(b, value);
    b++;

    for (uint64_t i = 0; i < n; i++)
    {
        j = SA->get_value(i);
        SA->set_value(i, ~j);

        if (0 < j)
        {
            j--;
            c0 = T->get_value(T_base + j + 1);
            if (c0 != c1)
            {
                B->set_value(B_base + c1, b);
                c1 = c0;
                b = B->get_value(B_base + c1);
            }

            value = ((0 < j) && (T->get_value(T_base + j) < c1)) ? ~j : j;
            SA->set_value(b, value);
            b++;
        }
    }

    // compute the SAs
    if (C_base == B_base)
    {
        get_counts(T, T_base, C, C_base, n, k);
    }

    get_buckets(C, C_base, B, B_base, k, true);

    c1 = 0;
    b = B->get_value(B_base + c1);
    for (int64_t i = n - 1; 0 <= i; i--)
    {
        j = SA->get_value(i);

        if (0 < j)
        {
            j--;
            c0 = T->get_value(T_base + j + 1);
            if (c0 != c1)
            {
                B->set_value(B_base + c1, b);
                c1 = c0;
                b = B->get_value(B_base + c1);
            }

            value = ((j == 0) || (T->get_value(T_base + j) > c1)) ? ~j : j;
            b--;
            SA->set_value(b, value);
        }
        else
        {
            SA->set_value(i, ~j);
        }
    }
}

void flbwt::sais_main(flbwt::PackedArray *T, uint64_t T_base, flbwt::PackedArray *SA, uint64_t fs, uint64_t n, uint64_t k)
{
    flbwt::PackedArray *C;  // pointer to C-vector
    flbwt::PackedArray *B;  // pointer to B-vector
    flbwt::PackedArray *BC; // pointer to BC-vector
    uint64_t C_base;        // start offset of C
    uint64_t B_base;        // start offset of B
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
        C_base = n;
        B_base = (k <= (fs - k)) ? C_base + k : C_base;
    }
    else
    {
        BC = new flbwt::PackedArray(k, n, false);
        C = B = BC;
        C_base = B_base = 0;
    }

    get_counts(T, T_base, C, C_base, n, k);
    get_buckets(C, C_base, B, B_base, k, true);

    for (uint64_t i = 0; i < n; i++)
        SA->set_value(i, 0);

    c = 0;
    c1 = T->get_value(T_base + n);
    for (int64_t i = n - 2; 0 <= i; i--)
    {
        if ((c0 = T->get_value(T_base + i + 1)) < (c1 + c))
        {
            c = 1;
        }
        else if (c != 0)
        {
            uint64_t index = B->get_value(B_base + c1) - 1;
            B->set_value(B_base + c1, index);
            SA->set_value(index, i + 1);
            c = 0;
        }

        c1 = c0;
    }

    induce_SA(T, T_base, SA, C, C_base, B, B_base, n, k);

    if (fs < k)
    {
        delete BC;
    }

    // compact all the sorted substrings into the first m items of SA
    m = 0;
    for (uint64_t i = 0; i < n; i++)
    {
        p = SA->get_value(i);

        if (0 < p)
        {
            int64_t prev = T->get_value(T_base + p);
            c0 = T->get_value(T_base + p + 1);

            if (prev > c0)
            {
                for (j = p + 1; (j < (int64_t)n); j++)
                {
                    c1 = T->get_value(T_base + j + 1);
                    if (c0 != c1)
                        break;
                }

                if ((j < (int64_t)n) && (c0 < c1))
                {
                    SA->set_value(m, p);
                    m++;
                }
            }
        }
    }

    // int the name array buffer
    for (uint64_t i = m; i < n; i++)
        SA->set_value(i, 0);

    // store the length of all substrings
    j = n;
    c = 0;
    c1 = T->get_value(T_base + n);
    for (int64_t i = n - 2; i >= 0; i--, c1 = c0)
    {
        c0 = T->get_value(T_base + i + 1);

        if (c0 < c1 + c)
        {
            c = 1;
        }
        else if (c != 0)
        {
            SA->set_value(m + ((i + 1) >> 1), j - i - 1);
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
        p = SA->get_value(i);
        int64_t index = m + (p >> 1);
        plen = SA->get_value(index);
        diff = 1;

        if (plen == qlen)
        {
            for (j = 0; j < plen; j++)
            {
                uint64_t pch = T->get_value(T_base + p + j + 1);
                uint64_t qch = T->get_value(T_base + q + j + 1);
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

        SA->set_value(index, name);
    }

    // STAGE 2: Solve the reduced problem. Recurse if names are not yet unique
    if (name < m)
    {
        RA = n + fs - m;
        for (int64_t i = n - 1, j = m - 1; m <= i; i--)
        {
            int64_t value = SA->get_value(i);

            if (value != 0)
            {
                SA->set_value(RA + j, value - 1);
                j--;
            }
        }

        flbwt::sais_main(SA, (RA - 1), SA, fs + n - m * 2, m, name);

        for (int64_t i = n - 2, j = m - 1, c = 0, c1 = T->get_value(T_base + n); 0 <= i; i--, c1 = c0)
        {
            c0 = T->get_value(T_base + i + 1);
            if (c0 < c1 + c)
            {
                c = 1;
            }
            else if (c != 0)
            {
                SA->set_value(RA + j, i + 1);
                j--;
                c = 0;
            }
        }

        for (int64_t i = 0; i < m; i++)
        {
            int64_t index = SA->get_value(i);
            int64_t value = SA->get_value(RA + index);
            SA->set_value(i, value);
        }
    }

    // STAGE 3: Induce the result for the original probelm
    if (k <= fs)
    {
        C = B = SA;
        C_base = n;
        B_base = (k <= (fs - k)) ? C_base + k: C_base;
    }
    else
    {
        BC = new PackedArray(k, n, false);
        C = B = BC;
        C_base = B_base = 0;
    }

    // put all LMS characters into their buckets
    get_counts(T, T_base, C, C_base, n, k);
    get_buckets(C, C_base, B, B_base, k, true);

    for (uint64_t i = m; i < n; i++)
        SA->set_value(i, 0);

    for (int64_t i = m - 1; 0 <= i; i--)
    {
        j = SA->get_value(i);
        SA->set_value(i, 0);

        int64_t index = T->get_value(T_base + j + 1);
        int64_t value = B->get_value(B_base + index ) - 1;
        B->set_value(B_base + index, value);
        SA->set_value(value, j);
    }

    induce_SA(T, T_base, SA, C, C_base, B, B_base, n, k);

    if (fs < k)
    {
        delete BC;
    }

}