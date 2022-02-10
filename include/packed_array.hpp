#ifndef FLBWT_PACKED_ARRAY_HPP
#define FLBWT_PACKED_ARRAY_HPP

#include <stdint.h>
#include <limits.h>

// https://stackoverflow.com/questions/22899466/this-declaration-has-no-storage-class-or-type-specifier-in-c
#define BIT_MASK(__TYPE__, __ONE_COUNT__) \
    ((__TYPE__) (-((__ONE_COUNT__) != 0))) \
    & (((__TYPE__) -1) >> ((sizeof(__TYPE__) * CHAR_BIT) - (__ONE_COUNT__)))

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
     * @brief Construct a new PackedArray object
     * 
     * @param length length of the PackedArray
     * @param support_negative_integers should the array support negative integers
     * @param integer_bits bits used by single integer in PackedArray
     */
    PackedArray(uint64_t length, bool support_negative_integers, uint8_t integer_bits);

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
     * @brief Store value at index in PackedArray.
     * 
     * @param index index in PackedArray where value is stored
     * @param value value to be stored
     */
    void set_value(uint64_t index, int64_t value);

    /**
     * @brief Get value stored at index in PackedArray.
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
    bool negative_integers;     // should the array support negative integers
    uint64_t arr_length;        // length of raw data of arr
    uint64_t signs_length;      // length of raw data of signs
};

}

#endif