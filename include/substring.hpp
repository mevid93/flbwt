#ifndef FLBWT_SUBSTRING_HPP
#define FLBWT_SUBSTRING_HPP

#include <stdint.h>

/**
 * @brief Data structure representing a single of S* substring.
 * If length of the substring is at most log_n(alphabet_size),
 * we directly store it and the operations are done O(1) time.
 * Otherwise, we store the position and length of the substring.
 */
class Substring
{
public:
    /**
     * @brief Construct a new Substring object
     * 
     * @param p pointer to the beginning of the substring
     * @param n length of the substring
     */
    Substring(const uint8_t *p, const uint64_t n);

    /**
     * @brief Store character c at the end of the substring.
     * 
     * @param c character to be pushed back of the substring
     */
    void push_back(const uint8_t c);

    /**
     * @brief Return the character at the front of the substring
     * and remove it. Returns NULL if substring is empty
     * 
     * @return character at the front of the substring
     */
    const char *pop_front();

    /**
     * @brief Reverse the order of the substring.
     */
    void reverse();

    /**
     * @brief Destroy the Substring object.
     */
    ~Substring();

private:
    bool is_reversed; // flag telling if the substring is reversed or not
    uint8_t *s;       // pointer to the substring
};

#endif