#include <iostream>
#include <stdlib.h>
#include "flbwt.hpp"
#include "utility.hpp"

// algorithm 1 from the research paper (modified induced sorting)
uint8_t *bwt_is(const uint8_t *T, const uint64_t n, const uint8_t k);

// allocate memory for the suffix array
flbwt::PackedArray *create_empty_suffix_array(flbwt::Container *container);

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
    flbwt::Container *container = flbwt::extract_LMS_strings(T, n, k);

    // sort the S*substrings and name them
    uint64_t *S = flbwt::sort_LMS_strings(T, container);

    // get new shortened string T1
    flbwt::PackedArray *T1 = flbwt::create_shortened_string(T, n, container);

    // create a suffix array for T1
    flbwt::PackedArray *SA = create_empty_suffix_array(container);

    // sort suffixes using induced sorting (SAIS)

    // create BWT for T1

    // create BWT for the original input string T

    std::cout << "Number of S* substrings found: " << container->num_of_substrings << std::endl;
    std::cout << "Number of unique S* substrings found: " << container->num_of_unique_substrings << std::endl;
    std::cout << "Head string end: " << container->head_string_end << std::endl;

    delete container;
    delete[] S;
    delete T1;
    delete SA;

    return NULL;
}

#define TYPE_L 1
#define TYPE_S 0

flbwt::Container *flbwt::extract_LMS_strings(const uint8_t *T, const uint64_t n, const uint8_t k)
{
    /* Initialize the result data structure */
    flbwt::Container *container = new Container();
    container->n = n;
    container->k = k;

    /* if T is trivial, then return the result immediately */
    if (n <= 2)
        return container;

    /* initialize hash table where unique substrings are stored */
    container->hashtable = new HashTable(67777, n, k);

    /* 
    The first S* substring is at location T[n] but it is ignored here.
    The next to last character is always of TYPE_L.
    Each S* substring in T can be denoted as T[p...q].
    */
    int previous_type = TYPE_L; // type of the previous character
    uint64_t p;                 // starting position of S* substring
    uint64_t q = n - 1;         // ending position of S* substring

    /* scan the input string from right to left and save the S* substrings */
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

                container->num_of_substrings++;
                container->c_substr_counts[T[p]]++;

                // insert unique substrings into hashtable
                if (container->hashtable->insert_string(T, q - p + 1, p))
                    container->num_of_unique_substrings++;

                q = p;
            }

            previous_type = TYPE_L;
        }

        if (i == 0)
            break; // to prevent integer underflow
    }

    // save the ending position of head strign
    container->head_string_end = p;

    /* all done, return the results */
    return container;
}

uint64_t *flbwt::sort_LMS_strings(const uint8_t *T, flbwt::Container *container)
{
    // array s will hold hashtable positions of sorted S* substrings
    uint64_t *s = new uint64_t[container->num_of_unique_substrings + 2];

    uint64_t p, i, j, l, m;

    j = 1; // j == 0 is for the head string T[0..p]
    for (i = 0; i < container->hashtable->HTSIZE; i++)
    {
        p = container->hashtable->head[i];
        while (p != 0)
        {
            l = container->hashtable->get_length(p);
            if (l == 0) // last string for hashtable index
                break;
            if (l == 1) // get position of the next element
            {
                p = container->hashtable->get_pointer(p);
                continue;
            }
            s[j++] = p;
            p = container->hashtable->skip_string(p);
        }
    }

    m = j - 1;

    // sort the substrings by using quick sort
    auto LMS_comparison = [T, container](uint64_t index_1, uint64_t index_2)
    {
        uint64_t l1 = container->hashtable->get_length(index_1);
        uint64_t l2 = container->hashtable->get_length(index_2);
        int c1, c2;

        uint64_t n = 0;
        while (l1 > 0 && l2 > 0)
        {
            // get next characters
            c1 = container->hashtable->get_nth_character(T, index_1, n);
            c2 = container->hashtable->get_nth_character(T, index_2, n);
            n++;

            if (c1 != c2)
                break;
            l1--;
            l2--;
        }

        if (l1 == 0)
            return true;
        if (l2 == 0)
            return false;
        return c1 - c2 < 0;
    };

    std::sort(s + 1, s + 1 + m, LMS_comparison);

    // assign the names for the substrings (fill them to hashtable)
    for (i = 1; i <= m; i++)
    {
        p = s[i];
        container->hashtable->set_name(p, i);
    }

    // add the head substring T[0..p] and last S* substring T[n] --> needed for BWT
    uint64_t bufsize = container->hashtable->buf->bit_size();
    container->hashtable->buf->bit_resize(bufsize + 300); // 300 bits plenty enough to store two substrings
    l = container->head_string_end + 1;
    uint64_t pos = bufsize;

    // add the head string
    if (l <= container->hashtable->SS_LIMIT)
    {
        s[container->num_of_unique_substrings + 1] = pos;
        container->hashtable->buf->set_int(pos, T[0] + 1, container->hashtable->SW_BITS);
        pos += container->hashtable->SW_BITS;
        (*container->hashtable->buf)[pos] = 0;
        pos += 1;
        container->hashtable->buf->set_int(pos, l, container->hashtable->SS_BITS);
        pos += container->hashtable->SS_BITS;
        for (i = 0; i < l; i++)
        {
            container->hashtable->buf->set_int(pos, T[0 + i], 8);
            pos += 8;
        }
        container->hashtable->buf->set_int(pos, container->num_of_unique_substrings + 1, container->hashtable->NAME_BITS);
        pos += container->hashtable->NAME_BITS;
    }
    else
    {
        s[container->num_of_unique_substrings + 1] = pos;
        container->hashtable->buf->set_int(pos, T[0] + 1, container->hashtable->SW_BITS);
        pos += container->hashtable->SW_BITS;
        (*container->hashtable->buf)[pos] = 1;
        pos += 1;
        container->hashtable->buf->set_int(pos, 0, container->hashtable->LSI_BITS);
        pos += container->hashtable->LSI_BITS;
        container->hashtable->buf->set_int(pos, l, container->hashtable->LSL_BITS);
        pos += container->hashtable->LSL_BITS;
        container->hashtable->buf->set_int(pos, container->num_of_unique_substrings + 1, container->hashtable->NAME_BITS);
        pos += container->hashtable->NAME_BITS;
    }

    // add the last S* substring
    s[0] = pos;
    pos += container->hashtable->SW_BITS;
    (*container->hashtable->buf)[pos] = 0;
    pos += 1;
    container->hashtable->buf->set_int(pos, 1, container->hashtable->SS_BITS);
    pos += container->hashtable->SS_BITS;
    container->hashtable->buf->set_int(pos, 0, 8);
    pos += 8;
    container->hashtable->buf->set_int(pos, 0, container->hashtable->NAME_BITS);
    pos += container->hashtable->NAME_BITS;
    container->hashtable->buf->set_int(pos, 0, 64); // reset the following bits

    return s;
}

flbwt::PackedArray *flbwt::create_shortened_string(const uint8_t *T, const uint64_t n, flbwt::Container *container)
{
    // define how many substrings there is in total
    uint64_t total_substring_count = container->num_of_substrings + 2;

    // maximum name of any substring
    uint64_t max_name = container->num_of_unique_substrings + 1;
    // number of bits required to store a name
    uint8_t bits_for_name = flbwt::position_of_msb(max_name);
    flbwt::PackedArray *T1 = new PackedArray(total_substring_count, bits_for_name);

    int previous_type = TYPE_L; // type of the previous character
    uint64_t p;                 // starting position of S* substring
    uint64_t q = n - 1;         // ending position of S* substring

    uint64_t j = total_substring_count - 1;
    T1->pa_set(j--, 0);

    /* scan the input string from right to left and save the S* substrings */
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
                uint64_t name = container->hashtable->find_name(T, q - p + 1, p);

                // insert name to T1
                T1->pa_set(j--, name);

                q = p;
            }

            previous_type = TYPE_L;
        }

        if (i == 0)
            break; // to prevent integer underflow
    }

    T1->pa_set(0, max_name);

    return T1;
}

flbwt::PackedArray *create_empty_suffix_array(flbwt::Container *container)
{
    // define how many substrings there is in total
    uint64_t total_substring_count = container->num_of_substrings + 2;

    // maximum name of any substring
    uint64_t max_name = container->num_of_unique_substrings + 1;

    // number of bits required to store a name
    uint8_t bits_for_name = flbwt::position_of_msb(max_name);

    // create suffix array and return it
    flbwt::PackedArray *T1 = new flbwt::PackedArray(total_substring_count, bits_for_name);

    return T1;
}