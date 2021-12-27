#ifndef FLBWT_BUCKET_HPP
#define FLBWT_BUCKET_HPP

#include <stdint.h>
#include <sdsl/bit_vectors.hpp>

namespace flbwt
{

/**
 * @brief Data structure that stores a set of S* substrings 
 * starting with character c.
 */
class Bucket
{
public:
    uint8_t c;                  // starting character of each substring in the bucket
    uint64_t bits;              // length of the bucket in bits
    uint64_t strings;           // number of substrings stored in the bucket (or to be stored)
    sdsl::bit_vector *b;        // bit array to store substrings

    /**
     * @brief Construct a new Bucket object.
     * 
     * @param c starting character of each substring in the bucket.
     * @param n number of bits required to store all S* substrings.
     */
    Bucket(const uint8_t c, const uint64_t n);

    /**
     * @brief Insert S* substring to next available position in the bucket.
     * 
     * @param T original input string from which the substring is extracted
     * @param n length of the input string
     * @param k alphabet size
     * @param p starting position of the substring
     * @param l length of the substring
     */
    void insert_substring(const uint8_t *T, const uint64_t n, const uint8_t k, const uint64_t p, const uint64_t l);

    /**
     * @brief Destroy the Bucket object
     */
    ~Bucket();

private:
    uint64_t *next_position;    // next available position for substring
};

}

#endif