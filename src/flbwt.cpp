#include <algorithm>
#include <iostream>
// #include <time.h>
#include <stdlib.h>
#include "flbwt.hpp"
#include "utility.hpp"
#include "induce40bit.hpp"
#include "induce48bit.hpp"
#include "induce56bit.hpp"
#include "induce64bit.hpp"
#include "sais32bit.hpp"
#include "sais40bit.hpp"
#include "sais48bit.hpp"
#include "sais56bit.hpp"
#include "sais64bit.hpp"

/**
 * @brief Algorithm 1 from the research paper (simplified version).
 * 
 * @param T input string
 * @param n input string length
 * @param free_T should the input string be freed
 * @return flbwt::BWT_result* result
 */
flbwt::BWT_result *bwt_is(uint8_t *T, const uint64_t n, bool free_T);

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
    // clock_t begin = clock();
    flbwt::BWT_result *B = flbwt::bwt_string(T, n, true);
    // clock_t end = clock();
    // std::cout << "Running time: " << ((double)(end - begin) / CLOCKS_PER_SEC) << std::endl;

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

    // Sort the S*substrings and name them
    uint8_t **S = flbwt::sort_LMS_strings(T, container);

    // Get new shortened string T1
    flbwt::PackedArray *T1 = flbwt::create_shortened_string(T, n, container);

    // Release T if user allows it --> lower memory usage
    if (free_T)
    {
        free(T);
        T = NULL;
    }

    // Create a suffix array for T1
    // Arrays of different sizes are used to store the SA
    int8_t *SA_8bit = NULL;
    int16_t *SA_16bit = NULL;
    uint16_t *SA_u16bit = NULL;
    int32_t *SA_32bit = NULL;
    uint32_t *SA_u32bit = NULL;
    int64_t *SA_64bit = NULL;

    uint64_t total_substring_count = container->num_of_substrings + 2;
    uint64_t max_value = container->sa_max_value;
    if (total_substring_count > max_value)
        max_value = total_substring_count;

    uint8_t bits = flbwt::position_of_msb(max_value);
    uint64_t T1_length = container->num_of_substrings + 1;
    uint64_t k = container->num_of_unique_substrings + 2;

    flbwt::BWT_result *BWT = NULL;

    uint64_t l;
    uint8_t *q;
    uint64_t p;

    if (bits < 32U)
    { // SA can be stored into array of 32 bit integers

        // Compute SA
        SA_32bit = new int32_t[total_substring_count];
        flbwt::sais_32bit((uint8_t *)T1->get_raw_arr_pointer(), SA_32bit, 0, T1_length, k, T1->get_integer_bits());

        // Compute BWT for shortened string
        for (uint64_t i = 0; i < container->num_of_substrings + 1; ++i)
            ++SA_32bit[i];

        for (uint64_t i = 0; i < container->num_of_substrings + 1; i++)
        {
            p = SA_32bit[i];
            q = S[T1->get_value(p - 1)];
            l = container->hashtable->get_length(q);
            uint64_t value = container->hashtable->get_first_character_pointer(q) + l - 1 - container->bwp_base;
            SA_32bit[i] = value;
        }

        // Release resources that are no longer needed
        free(S);
        delete T1;

        // Create BWT for the original input string T (SA_32bit is deleted in this function)
        BWT = flbwt::induce_bwt_32bit(SA_32bit, container);
    }
    else if (bits < 40U)
    { // SA can be stored into two integer arrays (32 + 8 bits)

        // Compute SA
        SA_u32bit = new uint32_t[total_substring_count]; // first 32 bits
        SA_8bit = new int8_t[total_substring_count];     // 8 most significant bits
        flbwt::sais_40bit((uint8_t *)T1->get_raw_arr_pointer(), NULL, NULL, SA_u32bit, SA_8bit, 0, T1_length, k, T1->get_integer_bits());

        // Compute BWT for shortened string
        for (uint64_t i = 0; i < container->num_of_substrings + 1; ++i)
        {
            int64_t tmp = flbwt::get_40bit_value(SA_u32bit, SA_8bit, i);
            flbwt::set_40bit_value(SA_u32bit, SA_8bit, i, tmp + 1);
        }

        for (uint64_t i = 0; i < container->num_of_substrings + 1; i++)
        {
            p = flbwt::get_40bit_value(SA_u32bit, SA_8bit, i);
            q = S[T1->get_value(p - 1)];
            l = container->hashtable->get_length(q);
            uint64_t value = container->hashtable->get_first_character_pointer(q) + l - 1 - container->bwp_base;
            flbwt::set_40bit_value(SA_u32bit, SA_8bit, i, value);
        }

        // Release resources that are no longer needed
        free(S);
        delete T1;

        // Create BWT for the original input string T (SA_u32bit and SA_8bit are deleted in this function)
        BWT = flbwt::induce_bwt_40bit(SA_u32bit, SA_8bit, container);
    }
    else if (bits < 48U)
    { // SA can be stored into two integer arrays (32 + 16 bits)

        // Compute SA
        SA_u32bit = new uint32_t[total_substring_count]; // first 32 bits
        SA_16bit = new int16_t[total_substring_count];   // 16 most significant bits
        flbwt::sais_48bit((uint8_t *)T1->get_raw_arr_pointer(), NULL, NULL, SA_u32bit, SA_16bit, 0, T1_length, k, T1->get_integer_bits());

        // Compute BWT for shortened string
        for (uint64_t i = 0; i < container->num_of_substrings + 1; ++i)
        {
            int64_t tmp = flbwt::get_48bit_value(SA_u32bit, SA_16bit, i);
            flbwt::set_48bit_value(SA_u32bit, SA_16bit, i, tmp + 1);
        }

        for (uint64_t i = 0; i < container->num_of_substrings + 1; i++)
        {
            p = flbwt::get_48bit_value(SA_u32bit, SA_16bit, i);
            q = S[T1->get_value(p - 1)];
            l = container->hashtable->get_length(q);
            uint64_t value = container->hashtable->get_first_character_pointer(q) + l - 1 - container->bwp_base;
            flbwt::set_48bit_value(SA_u32bit, SA_16bit, i, value);
        }

        // Release resources that are no longer needed
        free(S);
        delete T1;

        // Create BWT for the original input string T (SA_u32bit and SA_16bit are deleted in this function)
        BWT = flbwt::induce_bwt_48bit(SA_u32bit, SA_16bit, container);
    }
    else if (bits < 56U)
    { // SA can be stored into three integer arrays (32 + 16 + 8bits)

        // Compute SA
        SA_u32bit = new uint32_t[total_substring_count]; // first 32 bits
        SA_u16bit = new uint16_t[total_substring_count]; // 16 middle bits
        SA_8bit = new int8_t[total_substring_count];     // 8 most significant bits
        flbwt::sais_56bit((uint8_t *)T1->get_raw_arr_pointer(), NULL, NULL, NULL, SA_u32bit, SA_u16bit, SA_8bit, 0, T1_length, k, T1->get_integer_bits());

        // Compute BWT for shortened string
        for (uint64_t i = 0; i < container->num_of_substrings + 1; ++i)
        {
            int64_t tmp = flbwt::get_56bit_value(SA_u32bit, SA_u16bit, SA_8bit, i);
            flbwt::set_56bit_value(SA_u32bit, SA_u16bit, SA_8bit, i, tmp + 1);
        }

        for (uint64_t i = 0; i < container->num_of_substrings + 1; i++)
        {
            p = flbwt::get_56bit_value(SA_u32bit, SA_u16bit, SA_8bit, i);
            q = S[T1->get_value(p - 1)];
            l = container->hashtable->get_length(q);
            uint64_t value = container->hashtable->get_first_character_pointer(q) + l - 1 - container->bwp_base;
            flbwt::set_56bit_value(SA_u32bit, SA_u16bit, SA_8bit, i, value);
        }

        // Release resources that are no longer needed
        free(S);
        delete T1;

        // Create BWT for the original input string T (SA_u32bit, SA_u16bit and SA_8bit are deleted in this function)
        BWT = flbwt::induce_bwt_56bit(SA_u32bit, SA_u16bit, SA_8bit, container);
    }
    else if (bits < 64)
    { // SA can be stored into array of 64 bit integers

        // Compute SA
        SA_64bit = new int64_t[total_substring_count];
        flbwt::sais_64bit((uint8_t *)T1->get_raw_arr_pointer(), SA_64bit, 0, T1_length, k, T1->get_integer_bits());

        // Compute BWT for shortened string
        for (uint64_t i = 0; i < container->num_of_substrings + 1; ++i)
            ++SA_64bit[i];

        for (uint64_t i = 0; i < container->num_of_substrings + 1; i++)
        {
            p = SA_64bit[i];
            q = S[T1->get_value(p - 1)];
            l = container->hashtable->get_length(q);
            uint64_t value = container->hashtable->get_first_character_pointer(q) + l - 1 - container->bwp_base;
            SA_64bit[i] = value;
        }

        // Release resources that are no longer needed
        free(S);
        delete T1;

        // Create BWT for the original input string T (SA_32bit is deleted in this function)
        BWT = flbwt::induce_bwt_64bit(SA_64bit, container);
    }

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
    uint64_t p = n;
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
    uint8_t bits = flbwt::position_of_msb(max_name);
    // number of bits required to store a name
    PackedArray *T1 = new PackedArray(total_substring_count, bits);

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
