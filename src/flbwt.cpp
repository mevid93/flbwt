#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include "flbwt.hpp"
#include "utility.hpp"
#include "sais.hpp"
#include <ctime>

/**
 * @brief Algorithm 1 from the research paper (simplified version).
 * 
 * @param T input string
 * @param n input string length
 * @param free_T should the input string be freed
 * @return flbwt::BWT_result* result
 */
flbwt::BWT_result *bwt_is(uint8_t *T, const uint64_t n, bool free_T);

/**
 * @brief Create a empty suffix array object.
 * 
 * @param container container object
 * @return flbwt::PackedArray* empty suffic array
 */
flbwt::PackedArray *create_empty_suffix_array(flbwt::Container *container);

void flbwt::bwt_file(const char *input_filename, const char *output_filename)
{
    // Read content from the input file
    FILE *fp = fopen(input_filename, "r"); // file I/O stream
    uint64_t n;                            // length of the file content
    uint8_t *T;                            // file content string

    if (fp == NULL)
        throw std::invalid_argument("fopen failed(): Could not open input file");

    fseek(fp, 0L, SEEK_END);
    n = ftell(fp);
    rewind(fp);

    T = (uint8_t *)malloc((n + 1) * sizeof(uint8_t));

    if (!T)
    {
        fclose(fp);
        throw std::runtime_error("T* malloc failed(): Could not allocate memory");
    }

    if (fread(T, 1, n, fp) <= 0)
        throw std::runtime_error("fread failed(): Could not read input file");
    fclose(fp);

    // Input string should end with '\0' --> so the following assignment is ok.
    T[n] = '\0';

    // Construct the bwt for input string
    std::clock_t start;
    double duration;
    start = std::clock();
    flbwt::BWT_result *B = flbwt::bwt_string(T, n, true);
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    std::cout << "Duration: " << duration << "s" << std::endl;

    // Write the bwt to the output file */
    fp = fopen(output_filename, "wb");

    if (fp == NULL)
        throw std::invalid_argument("fopen failed(): Could not open output file");

    if (B != NULL && B->BWT != NULL)
    {
        // Write rank of the last character to the first 64 bits
        uint8_t *rank = new uint8_t[8];
        rank[0] = (0xff00000000000000 & B->last) >> 56;
        rank[1] = (0x00ff000000000000 & B->last) >> 48;
        rank[2] = (0x0000ff0000000000 & B->last) >> 40;
        rank[3] = (0x000000ff00000000 & B->last) >> 32;
        rank[4] = (0x00000000ff000000 & B->last) >> 24;
        rank[5] = (0x0000000000ff0000 & B->last) >> 16;
        rank[6] = (0x000000000000ff00 & B->last) >> 8;
        rank[7] = 0x00000000000000ff & B->last;
        fwrite(rank, sizeof(uint8_t), 8, fp);
        delete[] rank;

        // Write other content (BWT)
        if (B->last == 0)
        {
            fwrite(B->BWT + 1, sizeof(uint8_t), n, fp);
        }
        else if (B->last == n)
        {
            fwrite(B->BWT, sizeof(uint8_t), n, fp);
        }
        else
        {
            fwrite(B->BWT, sizeof(uint8_t), B->last, fp);
            fwrite(B->BWT + B->last + 1, sizeof(uint8_t), n - B->last, fp);
        }
    }

    fclose(fp);

    // Release result resources
    if (B != NULL)
    {
        if (B->BWT != NULL)
            delete[] B->BWT;
        free(B);
    }
}

flbwt::BWT_result *flbwt::bwt_string(uint8_t *T, const uint64_t n, bool free_T)
{
    // Check that input string is not NULL and length is greater than 0
    if (T == NULL || n <= 0)
        throw std::invalid_argument("bwt_string failed(): Invalid parameters");

    // Call the bwt construction with induced sorting
    return bwt_is(T, n, free_T);
}

flbwt::BWT_result *bwt_is(uint8_t *T, const uint64_t n, bool free_T)
{
    // Decompose the input string into S* substrings
    flbwt::Container *container = flbwt::extract_LMS_strings(T, n);

    // sort the S*substrings and name them
    uint8_t **S = flbwt::sort_LMS_strings(T, container);

    // get new shortened string T1
    flbwt::PackedArray *T1 = flbwt::create_shortened_string(T, n, container);

    // release T if user allows it --> lower memory usage
    if (free_T)
    {
        free(T);
        T = NULL;
    }

    // create a suffix array for T1
    flbwt::PackedArray *SA = create_empty_suffix_array(container);

    // sort suffixes using induced sorting (SAIS)
    flbwt::sais_main(T1, 0, SA, 0, container->num_of_substrings + 1, container->num_of_unique_substrings + 2);

    // create bwt for T1?
    for (uint64_t i = 0; i < container->num_of_substrings + 1; i++)
    {
        uint64_t val = SA->get_value(i);
        SA->set_value(i, val + 1);
    }

    for (uint64_t i = 0; i < container->num_of_substrings + 1; i++)
    {
        uint64_t l;
        uint8_t *q;
        uint64_t p;
        p = SA->get_value(i);
        q = S[T1->get_value(p - 1)];
        l = container->hashtable->get_length(q);
        uint64_t value = container->hashtable->get_first_character_pointer(q) + l - 1 - container->bwp_base;
        SA->set_value(i, value);
    }

    // release resources that are no longer needed
    free(S);
    delete T1;

    // create BWT for the original input string T (SA is deleted in this function)
    flbwt::BWT_result *BWT = flbwt::induce_bwt(SA, container);

    delete container;

    return BWT;
}

#define TYPE_L 1
#define TYPE_S 0

flbwt::Container *flbwt::extract_LMS_strings(uint8_t *T, const uint64_t n)
{
    // Initialize the result data structure
    flbwt::Container *container = new Container(n);

    // If T is trivial, then return the result immediately
    if (n <= 2)
        return container;

    // Initialize hash table where unique substrings are stored
    container->hashtable = new HashTable(67777, n);

    // The first S* substring is at location T[n] but it is ignored here.
    // The next to last character is always of TYPE_L.
    // Each S* substring in T can be denoted as T[p...q].
    int previous_type = TYPE_L;
    uint64_t p = 0;
    uint64_t q = n;
    ++container->M[0];
    ++container->M[T[n - 1] + 1];
    ++container->NL[T[n - 1] + 1];
    ++container->C[0];

    // Scan the input string from right to left and save the S* substrings
    for (int64_t i = n - 2; i >= 0; i--)
    {
        container->M[T[i] + 1]++;

        if (T[i] < T[i + 1])
        { // Character is TYPE_S
            previous_type = TYPE_S;
        }
        else if (T[i] > T[i + 1])
        { // Character is TYPE_L
            if (previous_type == TYPE_S)
            { // If previous character was TYPE_S, then new S* substring was found
                p = i + 1;
                ++container->C[T[p] + 1];
                ++container->num_of_substrings;

                // insert unique substrings into hashtable
                if (container->hashtable->insert_string(q - p + 1, &T[p]))
                    ++container->num_of_unique_substrings;

                q = p;
            }

            previous_type = TYPE_L;
            ++container->NL[T[i] + 1];
        }
        else
        { // Same as previous character
            if (previous_type == TYPE_L)
                ++container->NL[T[i] + 1];
        }
    }

    // Save the ending position of head strign
    container->head_string_end = p;

    return container;
}

uint8_t **flbwt::sort_LMS_strings(uint8_t *T, flbwt::Container *container)
{
    // array s will hold hashtable positions of sorted S* substrings
    uint8_t **s = (uint8_t **)malloc((container->num_of_unique_substrings + 2) * sizeof(uint8_t *));

    uint64_t p, i, j, l, m;
    uint8_t *r, *q;

    // allocate memory for head string and ending substring
    p = container->head_string_end;
    uint64_t bufsize = container->hashtable->bufsize;
    uint64_t space_required = 1 + 1 + container->hashtable->calculate_lenlen(p + 1) + (p + 1) + container->hashtable->NAME_BYTES;
    space_required += 1 + 1 + 1 + 1 + container->hashtable->NAME_BYTES;
    r = (uint8_t *)realloc(container->hashtable->buf, container->hashtable->bufsize + space_required);
    if (r != container->hashtable->buf)
        container->hashtable->buf = r;
    container->hashtable->bufsize += space_required;
    uint64_t pos = bufsize;

    j = 1; // j == 0 is for the head string T[0..p]
    for (i = 0; i < container->hashtable->HTSIZE; i++)
    {
        p = container->hashtable->head[i];
        while (p != 0)
        {
            r = &container->hashtable->buf[p];
            q = r;
            l = container->hashtable->get_length(r);
            if (l == 0) // last string for hashtable index
                break;
            if (l == 2) // get position of the next element block
            {
                p = container->hashtable->get_pointer(r);
                continue;
            }
            s[j++] = q;
            p += container->hashtable->string_info_length(r, l);
        }
    }

    m = j - 1;

    // sort the substrings by using quick sort
    auto LMS_comparison = [T, container](uint8_t *p1, uint8_t *p2)
    {
        uint64_t l1 = container->hashtable->get_length(p1);
        uint64_t l2 = container->hashtable->get_length(p2);
        int c1, c2;

        p1 = container->hashtable->get_first_character_pointer(p1);
        p2 = container->hashtable->get_first_character_pointer(p2);

        // compare other characters
        uint64_t n = 0;
        while (l1 > 0 && l2 > 0)
        {
            // get next characters
            c1 = *p1++;
            c2 = *p2++;
            n++;

            if (c1 != c2)
                break;
            l1--;
            l2--;
        }

        if (l1 == 0)
            return false;
        if (l2 == 0)
            return true;
        return c1 < c2;
    };

    std::sort(s + 1, s + 1 + m, LMS_comparison);

    // assign the names for the substrings (fill them to hashtable)
    for (i = 1; i <= m; i++)
    {
        q = s[i];
        container->hashtable->set_name(q, i);
    }

    // add the head substring T[0..p] and last S* substring T[n] --> needed for BWT
    r = &container->hashtable->buf[pos];
    s[container->num_of_unique_substrings + 1] = r;
    container->hashtable->set_length(r, container->head_string_end + 1);
    container->hashtable->set_name(r, container->num_of_unique_substrings + 1);
    r += 1 + container->hashtable->get_lenlen(r);
    *r++ = T[0] + 1;
    container->lastptr = r;
    for (i = 0; i < container->head_string_end + 1; i++)
    {
        *r++ = T[i];
    }
    r += container->hashtable->NAME_BYTES;

    // add the last S* substring
    s[0] = r;
    container->hashtable->set_length(r, 1);
    r += 1 + 1;
    *r++ = 0;
    r += 1;
    for (uint8_t i = 0; i < container->hashtable->NAME_BYTES; i++)
        *r++ = 0;

    // find maximum and minimum pointers --> needed later
    container->min_ptr = s[0];
    container->max_ptr = s[0];

    for (uint64_t i = 1; i < container->num_of_unique_substrings + 2; i++)
    {
        if (s[i] < container->min_ptr)
            container->min_ptr = s[i];
        if (s[i] > container->max_ptr)
            container->max_ptr = s[i];
    }

    container->bwp_width = flbwt::position_of_msb(container->max_ptr - container->min_ptr + 2);

    // also store the maximum value that will be user later --> needed for SA memory allocation
    container->bwp_base = container->min_ptr - 1;
    l = container->hashtable->get_length(container->max_ptr);
    container->sa_max_value = container->hashtable->get_first_character_pointer(container->max_ptr) + l - 1 - container->bwp_base;

    return s;
}

flbwt::PackedArray *flbwt::create_shortened_string(uint8_t *T, const uint64_t n, flbwt::Container *container)
{
    // define how many substrings there is in total
    uint64_t total_substring_count = container->num_of_substrings + 2;

    // maximum name of any substring
    uint64_t max_name = container->num_of_unique_substrings + 1;
    // number of bits required to store a name
    PackedArray *T1 = new PackedArray(total_substring_count, max_name, false);

    int previous_type = TYPE_L; // type of the previous character
    uint64_t p;                 // starting position of S* substring
    uint64_t q = n;             // ending position of S* substring

    uint64_t j = total_substring_count - 1;
    T1->set_value(j, 0);
    j--;

    // scan the input string from right to left and save the S* substrings
    for (uint64_t i = n - 2; i >= 0; i--)
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

                // find name of the substring
                uint64_t name = container->hashtable->find_name(q - p + 1, &T[p]);

                // insert name to T1
                T1->set_value(j, name);
                j--;

                q = p;
            }

            previous_type = TYPE_L;
        }

        if (i == 0)
            break; // to prevent integer underflow
    }

    T1->set_value(0, max_name);

    return T1;
}

flbwt::PackedArray *create_empty_suffix_array(flbwt::Container *container)
{
    // how many substrings there are in total
    uint64_t total_substring_count = container->num_of_substrings + 2;

    // maximum value that will be stored to SA in any point of the algorithm
    int64_t max_value = container->sa_max_value;

    // create packed array
    flbwt::PackedArray *SA = new flbwt::PackedArray(total_substring_count, max_value, true);

    return SA;
}