#include "flbwt.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

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
        perror("fopen() failed");
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0L, SEEK_END);
    n = ftell(fp);
    rewind(fp);

    T = malloc((n + 1) * sizeof(uint8_t));

    if (!T)
    {
        fclose(fp);
        perror("malloc() failed");
        exit(EXIT_FAILURE);
    }

    fread(T, 1, n, fp);
    fclose(fp);

    /* construct the bwt */
    uint8_t *B = bwt_string(T, n);

    /* write the bwt to the output file */
    fp = fopen(output_filename, "w+");

    if (fp == NULL)
    {
        perror("fopen() failed");
        exit(EXIT_FAILURE);
    }

    fputs(B, fp);
    fclose(fp);

    /* release resources */
    free(T);
    free(B);
}

uint8_t *bwt_string(const uint8_t *T, uint64_t n)
{
    /* check that input string is not null and n is greater than 0 */
    if (T == NULL || n <= 0)
    {
        printf("bwt_string() failed: Invalid parameters");
        exit(EXIT_FAILURE);
    }

    /* calculate the size of the alphabet */
    uint8_t k = calculate_alphabet_size(T, n);

    /* call the bwt construction with induced sorting */
    return bwt_is(T, n, k);
}

uint8_t *bwt_is(const uint8_t *T, uint64_t n, uint8_t k)
{
    // decompose the input string into S* substrings

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