#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "flbwt.hpp"
#include "utility.hpp"
#include "sequence.hpp"

// algorithm 1 from the research paper (modified induced sorting)
uint8_t *bwt_is(const uint8_t *T, uint64_t n, uint8_t k); 

void flbwt::bwt_file(const char *input_filename, const char *output_filename)
{
    /* read content from the input file */
    FILE *fp = fopen(input_filename, "r"); // file stream (input & output)
    uint64_t n;                            // length of the file content
    uint8_t *T;                            // file content string

    if (fp == NULL)
    {
        throw std::invalid_argument("fopen failed(): Could not open input file");
    }

    fseek(fp, 0L, SEEK_END);
    n = ftell(fp);
    rewind(fp);

    T = (uint8_t *)malloc((n + 1) * sizeof(uint8_t));

    if (!T)
    {
        fclose(fp);
        throw std::runtime_error("T* malloc failed(): Could not allocate memory");
    }

    fread(T, 1, n, fp);
    fclose(fp);

    /* construct the bwt */
    uint8_t *B = flbwt::bwt_string(T, n);

    /* write the bwt to the output file */
    fp = fopen(output_filename, "w+");

    if (fp == NULL)
    {
        throw std::invalid_argument("fopen failed(): Could not open output file");
    }

    if (B != NULL)
    {
        fwrite(B, sizeof(uint8_t), n * sizeof(uint8_t), fp);
    }

    fclose(fp);

    /* release resources */
    free(T);
    free(B);
}

uint8_t *flbwt::bwt_string(const uint8_t *T, const uint64_t n)
{
    /* check that input string is not null and n is greater than 0 */
    if (T == NULL || n <= 0)
    {
        std::cout << "bwt_string() failed: Invalid parameters" << std::endl;
        throw std::invalid_argument("bwt_string failed(): Invalid parameters");
    }

    /* calculate the size of the alphabet */
    uint8_t k = flbwt::calculate_alphabet_size(T, n);

    /* call the bwt construction with induced sorting */
    return bwt_is(T, n, k);
}

uint8_t *bwt_is(const uint8_t *T, const uint64_t n, const uint8_t k)
{
    // decompose the input string into S* substrings
    flbwt::BucketMap *bucket_map = flbwt::extract_LMS_strings(T, n, k);

    // TODO: store the qi value of each S* substring to lemma 1 data structure
    std::cout << "Number of S* substrings found: " << bucket_map->getTotalNumberOfSubstrings() << std::endl;
    flbwt::Sequence *sequence = new flbwt::Sequence(bucket_map->getTotalNumberOfSubstrings(), n * 255 + n);
    delete sequence;
    delete bucket_map;

    // sort the S*substrings

    // name S* substrings by using the result of Induce(R)
    // to get new shortened string T1

    // if each character in T1 is unique then directly
    // compute B1 from T1

    // else do the recursive call with shortened string T1

    // decode S* strings B1 into R'

    // calculate the final BWT by calling Induce(R')

    return NULL;
}

#define TYPE_L 1
#define TYPE_S 0

flbwt::BucketMap *flbwt::extract_LMS_strings(const uint8_t *T, const uint64_t n, const uint8_t k)
{
    /* 
    Count the number of bits required to store each S* substring
    to their corresponding c-bucket. After that initialize the buckets.
    */
    uint64_t *counts = flbwt::count_bits_for_buckets(T, n, k);

    /* initialize bucket map with the bit counts */
    flbwt::BucketMap *bucket_map = new flbwt::BucketMap(counts);
    delete[] counts;

    /* if T is trivial, then return the result immediately */
    if (n <= 2)
    {
        return bucket_map;
    }

    /* 
    The first S* substring is at location T[n] but it is ignored here.
    The next to last character is always of TYPE_L.
    Each S* substring in T can be denoted as T[p...q].
    */
    int previous_type = TYPE_L; // type of the previous character
    uint64_t p;                 // starting position of S* substring
    uint64_t q = n;             // ending position of S* substring

    /* scan the input string from right to left and save the S* substrings */
    for (int i = n - 2; i >= 0; i--)
    {
        // check if character is TYPE_S
        if (T[i] < T[i + 1])
        {
            previous_type = TYPE_S;
        }
        // check if character is TYPE_L
        else if (T[i] > T[i + 1])
        {
            // if previous character was TYPE_S, then new S* substring was found
            if (previous_type == TYPE_S)
            {
                p = i + 1;

                // save the S* substrings into buckets
                bucket_map->insert_substring(T, n, k, p, q - p + 1);

                q = p;
            }

            previous_type = TYPE_L;
        }
    }

    /* all done, return the results */
    return bucket_map;
}

uint64_t *flbwt::count_bits_for_buckets(const uint8_t *T, const uint64_t n, const uint8_t k)
{
    /* array to hold number of substrings starting with character c */
    uint64_t *counts = new uint64_t[256];
    std::fill_n(counts, 256, 0);

    /* if T is trivial, then return the result immediately */
    if (n == 2)
    {
        return counts;
    }

    /* 
    The first S* substring is at location T[n] but it is ignored here.
    The next to last character is always of TYPE_L.
    Each S* substring in T can be denoted as T[p...q].
    */
    int previous_type = TYPE_L; // type of the previous character
    uint64_t p;                 // starting position of S* substring
    uint64_t q = n;             // ending position of S* substring
    uint64_t LSI_BITS;          // number of bits required to store long substring index
    uint64_t LSL_BITS;          // number of bits required to store long substring length
    uint64_t SS_LIMIT;          // small substring length limit
    uint64_t SS_BITS;           // number of bits requires to store small substring length

    LSI_BITS = flbwt::position_of_msb(n);
    LSL_BITS = LSI_BITS;
    SS_LIMIT = flbwt::log2_64(n) / flbwt::log2_64(k);
    SS_BITS = flbwt::position_of_msb(SS_LIMIT);

    /* scan the input string from right to left and save the S* substrings */
    for (int i = n - 2; i >= 0; i--)
    {
        // check if character is TYPE_S
        if (T[i] < T[i + 1])
        {
            previous_type = TYPE_S;
        }
        // check if character is TYPE_L
        else if (T[i] > T[i + 1])
        {
            // if previous character was TYPE_S, then new S* substring was found
            if (previous_type == TYPE_S)
            {
                p = i + 1;

                // increase the corresponding c-bucket bit count
                if (q - p + 1 <= SS_LIMIT)
                {
                    counts[T[p]] += 1 + SS_BITS + (q - p + 1) * 8;
                }
                else
                {
                    counts[T[p]] += 1 + LSI_BITS + LSL_BITS;
                }

                q = p;
            }

            previous_type = TYPE_L;
        }
    }

    /* all done, return the results */
    return counts;
}