#ifndef FLBWT_HASHTABLE_HPP
#define FLBWT_HASHTABLE_HPP

#include <stdint.h>

namespace flbwt
{

    /**
     * Each unique substring will have the following format...
     * 1. length x in bytes (how many bytes need to store the length),
     * 2. x bytes (when combined --> total length),
     * 3. sentinel character (one byte), 
     * 4. substring characters (total length * 1 byte)
     * 5. bytes for name (when combined --> name)
     * 
     * POINTER to the next block is a special case with the following format:
     * 1. length x in bytes (how many bytes need to store the length),
     * 2. x bytes (when combined --> total length),
     * 3. total length is always TWO for pointers
     * 4. next 8 bytes store the position of the next element block
     */

    /**
     * @brief HashTable class for storing substrings.
     */
    class HashTable
    {
    public:
        uint64_t HTSIZE;             // hash table size
        uint64_t HBSIZE;             // how many bytes buf is increased when resized
        uint64_t *rest;              // array keeping track how much space there is left
        uint64_t *head;              // array keeping track of records (hash values are indices for this)
        uint8_t *buf;                // memory for storing the substrings
        uint64_t bufsize;            // size of the buf memory
        uint64_t collisions;         // number of collisions with other substrings
        uint8_t LENGTH_X_BYTES;      // how many bytes needed to store length X
        uint8_t NAME_BYTES;          // number of bytes needed for the name
        uint8_t SENTINEL_CHAR_BYTES; // number of bytes needed for sentinel

        /**
         * @brief Construct a new Hash Table object.
         * 
         * @param hash_table_size size of the table
         * @param n input string length
         */
        HashTable(const uint64_t hash_table_size, const uint64_t n);

        /**
         * @brief Insert substring to hashtable if it does not exists there yet.
         * Function returns 1 after succesfully inserting the string to the table.
         * If string is already in the table, return 0.
         * 
         * @param m length of the substring
         * @param p pointer to the beginning of the substring
         * @return uint8_t operation status
         */
        uint8_t insert_string(const uint64_t m, uint8_t *p);

        /**
         * @brief Function for calculating hash for substring.
         * 
         * @param m length of the substring
         * @param p pointer to the beginning of the substring
         * @return uint64_t hash
         */
        uint64_t hash_function(const uint64_t m, uint8_t *p);

        /**
         * @brief Get the length of substring.
         * 
         * @param p pointer to the position of the substring
         */
        uint64_t get_length(uint8_t *p);

        /**
         * @brief Set the length of substring..
         * 
         * @param p pointer to the position of substring
         * @param length length of the substring
         */
        void set_length(uint8_t *p, uint64_t length);

        /**
         * @brief Return total length of stored substring.
         * 
         * @param p position of the current string
         * @param length length of the substring
         */
        uint64_t string_info_length(uint8_t *p, uint64_t length);

        /**
         * @brief Get pointer to the beginning of the next substring block.
         */
        uint64_t get_pointer(uint8_t *p);

        /**
         * @brief Set pointer to next substring block.
         * 
         * @param p pointer to the pointer-substring
         * @param pointer pointer value to be stored
         */
        void set_pointer(uint8_t *p, uint64_t pointer);

        /**
         * @brief Get number of bytes needed to store the substring length.
         * 
         * @param p pointer to substring position
         * @return uin8_t number of bytes needed
         */
        uint8_t get_lenlen(uint8_t *p);

        /**
         * @brief Calculate number of bytes needed to store substring length.
         * 
         * @param m length of substring
         * @return uint8_t bytes needed
         */
        uint8_t calculate_lenlen(uint64_t m);

        /**
         * @brief Set the name of substring.
         * 
         * @param p pointer to substring
         * @param name name of the substring
         */
        void set_name(uint8_t *p, uint64_t name);

        /**
         * @brief Get the name of substring.
         * 
         * @param p pointer to substring
         */
        uint64_t get_name(uint8_t *p);

        /**
         * @brief Get the pointer that points to first character of a substring.
         * 
         * @param p pointer to substring
         * @return uint8_t* pointer to first charachter
         */
        uint8_t *get_first_character_pointer(uint8_t *p);

        /**
         * @brief Find the name of a substring.
         * 
         * @param m length of substring
         * @param p pointer to begining of substring (in original input string T)
         * @return uint64_t name of the substring
         */
        uint64_t find_name(uint64_t m, uint8_t *p);

        /**
         * @brief Destroy the HashTable object.
         */
        ~HashTable();
    };

}

#endif