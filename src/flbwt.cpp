#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "flbwt.hpp"
#include "utility.hpp"

// forward declarations
uint8_t *bwt_is(const uint8_t *T, uint64_t n, uint8_t k);

void bwt_file(const char *input_filename, const char *output_filename)
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
    uint8_t *B = bwt_string(T, n);

    /* write the bwt to the output file */
    fp = fopen(output_filename, "w+");

    if (fp == NULL)
    {
        throw std::invalid_argument("fopen failed(): Could not open output file");
    }

    fwrite(B, sizeof(uint8_t), n * sizeof(uint8_t), fp);
    fclose(fp);

    /* release resources */
    free(T);
    free(B);
}

uint8_t *bwt_string(const uint8_t *T, const uint64_t n)
{
    /* check that input string is not null and n is greater than 0 */
    if (T == NULL || n <= 0)
    {
        std::cout << "bwt_string() failed: Invalid parameters" << std::endl;
        throw std::invalid_argument("bwt_string failed(): Invalid parameters");
    }

    /* calculate the size of the alphabet */
    uint8_t k = calculate_alphabet_size(T, n);

    /* call the bwt construction with induced sorting */
    return bwt_is(T, n, k);
}

uint8_t *bwt_is(const uint8_t *T, const uint64_t n, const uint8_t k)
{
    // decompose the input string into S* substrings
    LMS_strings *lms = extract_LMS_strings(T, n);
    free(lms);

    // sort the S*substrings

    // name S* substrings by using the result of Induce(R)
    // to get new shortened string T1

    // if each character in T1 is unique then directly
    // compute B1 from T1

    // else do the recursive call with shortened string T1

    // decode S* strings B1 into R'

    // calculate the final BWT by calling Induce(R')
    uint8_t *B = (uint8_t *)malloc(n * sizeof(uint8_t));
    memcpy(B, T, n);
    return B;
}

#define TYPE_L 1
#define TYPE_S 0

LMS_strings *extract_LMS_strings(const uint8_t *T, const uint64_t n)
{
    /* initialize result variable */
    LMS_strings *lms = new LMS_strings{0};

    /* if T is trivial, then return the result immediately */
    if (n <= 2)
    {
        return lms;
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
                // TODO: save the S* substrings into buckets
                std::cout << "S* substring found... T[" << std::to_string(p) << "..." << std::to_string(q) << "]\n";
                lms->count++;
                q = p;
            }

            previous_type = TYPE_L;
        }
    }

    /* all done, return the results */
    return lms;
}