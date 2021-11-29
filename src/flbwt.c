#include "flbwt.h"
#include <stdio.h>
#include <stdlib.h>

void bwt_file(const char *input_filename, const char *output_filename)
{
    /* read content from the input file */
    FILE *fp = fopen(input_filename, "r"); // file stream
    uint64_t n;                            // length of the file content
    uint8_t *T;                            // content string

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

    /* TODO: Implement rest of it :) */

    /* release resources */
    free(T);
}

uint8_t *bwt_string(uint8_t *T, uint64_t n)
{
    return 0;
}