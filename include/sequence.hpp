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
    sdsl::bit_vector *upper_stream; // upper stream (stores upper lg(n) bits)
    sdsl::bit_vector *lower_stream; // lower stream (stores lower lg(U/n) bits)
    uint64_t upper_stream_length;   // length of the upper stream
    uint64_t lower_stream_length;   // lenght of the lower stream

    /**
     * @brief Construct a new Sequence object.
     * 
     * @param n number of integers to be stored
     * @param U upper limit for integers to be stored
     */
    Sequence(const uint64_t n, const uint64_t U);

    /**
     * @brief Store integer value to sequence. O(1) time complexity.
     * Each integer that is stored must be smaller than upper limit U.
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

private:
    uint64_t U;                       // max number of integers
    uint64_t n;                       // number of integers that can be stored to sequence
    uint8_t ubits;                    // how many bits of integer belong to upper stream
    uint8_t lbits;                    // how many bits of integer belong to lower stream
    sdsl::select_support_mcl<1> *sls; // auxiliary data type enabling O(1) get operations
    bool select_support_enabled;      // is select support

    /**
     * @brief Enable constant time O(1) operation by doing
     * O(n) time preprocessing.
     * 
     */
    void enable_select_operation();
};

#endif