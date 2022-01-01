#ifndef FLBWT_HASHTABLE_HPP
#define FLBWT_HASHTABLE_HPP

#include <stdint.h>
#include <sdsl/bit_vectors.hpp>

namespace flbwt
{

/**
 * Each unique substring will have the following format...
 * short substrings: 1 bit for type, 2..6 bits for length, m bytes for content, log(n) bits for name
 * long substrings: 1 bit for type, log(n) bits for index, log(n) bits for length, log(n) bits for name
 * If substring length = 1, it means that the following 64 bits point to the next substring
 * There is also an additional sentinel character (8 bits)
 */

/**
 * @brief HashTable for storing the S* substrings.
 */
class HashTable
{
public:
    uint64_t HTSIZE;            // hash table size
    uint64_t HBSIZE;            // how many bits the bit vector size is increased when resized
    uint64_t *rest;             // array keeping track how much space there is left 
    uint64_t *head;             // array keeping track of records (hash values are indices for this)
    sdsl::bit_vector *buf;      // bit vector where the substrings are stored
    uint64_t collisions;        // number of collisions with other substrings
    uint8_t LSI_BITS;           // number of bits needed for long substring index
    uint8_t LSL_BITS;           // number of bits needed for long substring length
    uint8_t SS_LIMIT;           // maximum length of short substring
    uint8_t SS_BITS;            // number of bits needed for short substring length
    uint8_t NAME_BITS;          // number of bits needed for the name
    uint8_t SW_BITS;            // number of bits needed for sentinel

    /**
     * @brief Construct a new Hash Table object.
     * 
     * @param hash_table_size size of the table
     * @param n input string length
     * @param k alphabet size
     */
    HashTable(const uint64_t hash_table_size, const uint64_t n, const uint8_t k);

    /**
     * @brief Insert substring to hashtable if it does not exists there yet.
     * Function returns 1 after succesfully inserting the string to the table.
     * If string is already in the table, return 0.
     * 
     * @param T input string
     * @param m length of the substring
     * @param p index of the substring (starting position)
     * @return uint8_t operation status
     */
    uint8_t insert_string(const uint8_t *T, const uint64_t m, const uint64_t p);


    /**
     * @brief Function for calculating hash for substring.
     * 
     * @param m length of the substring
     * @param p index of the beginning of the substring
     * @return uint64_t hash
     */
    uint64_t hash_function(const uint8_t *T, const uint64_t m, const uint64_t p);

    /**
     * @brief Get the length of substring stored at index idx in bit vector.
     */
    uint64_t get_length(uint64_t idx);

    /**
     * @brief Get the index of the next element from bit-vector by using "substring" at position idx.
     */
    uint64_t get_pointer(uint64_t idx);

    /**
     * @brief Return the index of substring that follows the substring at index idx.
     */
    uint64_t skip_string(uint64_t idx);

    /**
     * @brief Set the length of substring starting at position idx.
     * This function will also set the type bit.
     */
    void set_length(uint64_t idx, uint64_t length);

    /**
     * @brief Set the position (index) of the next element in bit-vector.
     */
    void set_pointer(uint64_t idx, uint64_t p);

    /**
     * @brief Set the index of long substring.
     */
    void set_position(uint64_t idx, uint64_t p);

    /**
     * @brief Get the index of long substring.
     */
    uint64_t get_position(uint64_t idx);

    /**
     * @brief Destroy the HashTable object.
     */
    ~HashTable();
};

}

#endif