#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include "flbwt.hpp"
#include "utility.hpp"
#include "sais.hpp"
#include "induce.hpp"

// algorithm 1 from the research paper (modified induced sorting)
uint8_t *bwt_is(uint8_t *T, const uint64_t n, bool free_T);

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

    // input string should end with '\0' --> so the following assignment is ok
    // if this is not done, valgrind gives warnings
    T[n] = '\0';

    /* construct the bwt */
    uint8_t *B = flbwt::bwt_string(T, n, true);

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

    if (B != NULL)
    {
        free(B);
    }
}

uint8_t *flbwt::bwt_string(uint8_t *T, const uint64_t n, bool free_T)
{
    // check that input string is not null and n is greater than 0
    if (T == NULL || n <= 0)
    {
        std::cout << "bwt_string() failed: Invalid parameters" << std::endl;
        throw std::invalid_argument("bwt_string failed(): Invalid parameters");
    }

    // call the bwt construction with induced sorting
    return bwt_is(T, n, free_T);
}

uint8_t *bwt_is(uint8_t *T, const uint64_t n, bool free_T)
{
    // decompose the input string into S* substrings
    flbwt::Container *container = flbwt::extract_LMS_strings(T, n);

    std::cout << "Number of S* substrings found: " << container->num_of_substrings << std::endl;
    std::cout << "Number of unique S* substrings found: " << container->num_of_unique_substrings << std::endl;
    std::cout << "Head string end: " << container->head_string_end << std::endl;

    // sort the S*substrings and name them
    uint8_t **S = flbwt::sort_LMS_strings(T, container);

    std::cout << "Substrings sorted by using quicksort! " << std::endl;

    // get new shortened string T1
    flbwt::PackedArray *T1 = flbwt::create_shortened_string(T, n, container);

    std::cout << "Shortened string created: T1.length = " << T1->get_length() << std::endl;

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

    std::cout << "SAIS computed!" << std::endl;

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

    // create BWT for the original input string T
    // flbwt::induce_bwt(SA, container);


    delete container;
    delete SA;

    return NULL;
}

#define TYPE_L 1
#define TYPE_S 0

flbwt::Container *flbwt::extract_LMS_strings(uint8_t *T, const uint64_t n)
{
    /* Initialize the result data structure */
    flbwt::Container *container = new Container();
    container->n = n;
    container->k = 0;

    /* if T is trivial, then return the result immediately */
    if (n <= 2)
        return container;

    /* initialize hash table where unique substrings are stored */
    container->hashtable = new HashTable(67777, n);

    /* 
    The first S* substring is at location T[n] but it is ignored here.
    The next to last character is always of TYPE_L.
    Each S* substring in T can be denoted as T[p...q].
    */
    int previous_type = TYPE_L;  // type of the previous character
    uint8_t alphabet[256] = {0}; // alphabet counting array
    uint64_t p;                  // starting position of S* substring
    uint64_t q = n;              // ending position of S* substring
    alphabet[0]++;
    alphabet[T[n - 1]]++;
    container->k++;

    /* scan the input string from right to left and save the S* substrings */
    for (uint64_t i = n - 2; i >= 0; i--)
    {
        // count alphabet size
        uint8_t val = T[i];
        if (alphabet[val] == 0)
        {
            alphabet[val]++;
            container->k++;
        }

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
                if (container->hashtable->insert_string(q - p + 1, &T[p]))
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

    // all done, return the results
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
    *r = T[0] + 1;
    container->hashtable->set_length(r, p + 1);
    container->hashtable->set_name(r, container->num_of_unique_substrings + 1);
    r = container->hashtable->get_first_character_pointer(r);
    for (i = 0; i < p + 1; i++)
    {
        *r++ = T[i];
    }
    r += container->hashtable->NAME_BYTES;

    // add the last S* substring
    s[0] = r;
    container->hashtable->set_length(r, 1);
    *r = 0;
    r += 1 + 1 + 1 + 1;
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