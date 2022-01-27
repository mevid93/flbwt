#ifndef FLBWT_PACKED_ARRAY_HPP
#define FLBWT_PACKED_ARRAY_HPP

#include <stdint.h>

namespace flbwt {

/**
 * @brief Array like data structure that stores integers space efficiently. 
 */
class PackedArray
{
public:
    

    /**
     * @brief Construct a new PackedArray object
     * 
     * @param length length of the PackedArray
     * @param max_integer largest integer (absolute value) that can be stored to PackedArray
     * @param support_negative_integers should the array support negative integers
     */
    PackedArray(uint64_t length, uint64_t max_integer, bool support_negative_integers);

    /**
     * @brief Get the length of the PackedArray.
     * 
     * @return uint64_t PackedArray length
     */
    uint64_t get_length();

    /**
     * @brief Get the number of bits needed by single integer.
     * 
     * @return uint8_t bits
     */
    uint8_t get_integer_bits();

    /**
     * @brief Check if PackedArray supports negative numbers.
     * 
     * @return true PackedArray supports negative numbers
     * @return false PackedArray does not support negative numbers
     */
    bool supports_negative_integers();

    /**
     * @brief Get the maximum integer that can be stored to PackedArray.
     * 
     * @return uint64_t max integer
     */
    uint64_t get_maximum_supported_integer();

    /**
     * @brief Set value of index in PackedArray.
     * 
     * @param index index of which value is set
     * @param value value to bet set
     */
    void set_value(uint64_t index, int64_t value);

    /**
     * @brief Get the value at index.
     * 
     * @param index index from which value is retrieved
     * @return int64_t value
     */
    int64_t get_value(uint64_t index);

    /**
     * @brief Get the pointer pointing to raw integer data.
     * 
     * @return uint64_t* 
     */
    uint64_t *get_raw_arr_pointer();

    /**
     * @brief Get the pointer pointing to raw signs data.
     * 
     * @return uint64_t* 
     */
    uint64_t *get_raw_signs_pointer();

    /**
     * @brief Destroy the PackedArray object
     */
    ~PackedArray();

private:
    uint64_t *arr;              // array where the integers are packed (without sign bit)
    uint64_t *signs;            // array where the sign bit is set
    uint64_t length;            // length of the packed array
    uint8_t integer_bits;       // number of bits for single integer
    uint64_t max_integer;       // max integer that can be stored to PackedArray
    bool negative_integers;     // should the array support negative integers
    uint64_t arr_length;        // length of raw data of arr
    uint64_t signs_length;      // length of raw data of signs
};

}

#endif