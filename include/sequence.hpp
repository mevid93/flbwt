#ifndef FLBWT_SEQUENCE_HPP
#define FLBWT_SEQUENCE_HPP

#include <stdint.h>
#include <sdsl/bit_vectors.hpp>

/**
 * @brief Data structure that stores a sequence of n integers 
 * such that 0 <= s1 < s2 < ... < sn < U in a bit stream
 * of n(2+lg(U/n)) bits.
 */
class Sequence
{
public:
    sdsl::bit_vector *upper_stream;
    uint64_t *lower_stream;
    uint64_t upper_stream_length;
    uint64_t lower_stream_length;

    /**
     * @brief Construct a new Sequence object.
     * 
     * @param n number of integers to be stored
     */
    Sequence(const uint64_t n);

    /**
     * @brief Store integer value to sequence. O(1) time complexity.
     * 
     * @param index index of the value to be stored
     * @param value value to be stored
     */
    void store_integer(const uint64_t index, const uint64_t integer);

    /**
     * @brief Get integer from by index;
     * 
     * @param index index of integer
     * @return uint64_t integer
     */
    uint64_t get_integer(const uint64_t index);

    /**
     * @brief Destroy the Sequence object
     * 
     */
    ~Sequence();
};

#endif