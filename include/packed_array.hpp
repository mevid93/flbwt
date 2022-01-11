#ifndef FLBWT_PACKED_ARRAY_HPP
#define FLBWT_PACKED_ARRAY_HPP

#include <stdint.h>

#define PBS 32
#define _logD_ 5
#define _D_ (1 << _logD_)

namespace flbwt
{

/**
 * @brief PackedArray for storing shortened string T1.
 */
class PackedArray
{
public:
    uint64_t n;     // number of characters (names)
    uint8_t w;      // character (name) width in bits
    uint32_t *b;    // content array

    /**
     * @brief Construct a new Packed Array object.
     * 
     * @param n number of characters (names)
     * @param w width of the character (name) in bits
     */
    PackedArray(uint64_t n, uint8_t w);

    /**
     * @brief Pack value to array.
     * 
     * @param i index of the value 
     * @param x value
     */
    void pa_set(uint64_t i, uint64_t x);

    /**
     * @brief Get value from the array.
     * 
     * @param i index of the value
     * @return uint64_t value
     */
    uint64_t pa_get(uint64_t i);

    /**
     * @brief Set the bits at index i to store value x.
     * 
     * @param b pointer to array
     * @param i index
     * @param x value
     */
    void set_bits(uint32_t *b, uint64_t i, uint64_t x);

    /**
     * @brief Get the value at index i.
     * 
     * @param b pointer to array
     * @param i index
     * @return uint64_t value
     */
    uint64_t get_bits(uint32_t *b, uint64_t i);

    /**
     * @brief Destroy the PackedArray object.
     */
    ~PackedArray();
};

}

#endif