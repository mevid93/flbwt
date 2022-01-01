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
    uint64_t strings;           // number of substrings stored in the bucket
    sdsl::bit_vector *b;        // bit array to store substrings

    /**
     * @brief Construct a new Bucket object.
     * 
     * @param c starting character
     * @param lsi_bits bits required to store the index of long substring
     * @param ss_limit maximum length of short substring
     */
    Bucket(const uint8_t c, const uint8_t lsi_bits, const uint8_t ss_limit);

    /**
     * @brief Insert S* substring to next available position in the bucket.
     * 
     * @param T original input string from which the substring is extracted
     * @param p starting position of the substring
     * @param l length of the substring
     */
    void insert_substring(const uint8_t *T, const uint64_t p, const uint64_t l);

    /**
     * @brief Set the next position to point at the beginning of the bucket.
     */
    void reset_next_position();

    /**
     * @brief Destroy the Bucket object
     */
    ~Bucket();

private:
    uint64_t next_position;     // next available position for substring
    uint8_t LSI_BITS;           // number of bits required to store long substring index
    uint8_t SS_LIMIT;           // small substring length limit
};

}

#endif