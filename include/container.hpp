#ifndef FLBWT_CONTAINER_HPP
#define FLBWT_CONTAINER_HPP

#include <stdint.h>
#include "hashtable.hpp"

namespace flbwt
{

/**
 * @brief Container class for storing the results of LMS-extraction.
 */
class Container
{
public:
    flbwt::HashTable *hashtable;           // hashtable for storing substrings
    uint64_t M[256 + 2];                   
    uint64_t M2[256 + 2];
    uint64_t M3[256 + 2];
    uint64_t C[256 + 2];
    uint64_t C2[256 + 2];
    uint64_t NL[256 + 2];
    uint64_t num_of_substrings;            // number of all substrings
    uint64_t num_of_unique_substrings;     // number of unique substrings
    uint64_t n;                            // length of the input string
    uint64_t k;                            // alphabet size
    uint64_t head_string_end;              // index of the last character of the head string T[0...p]
    uint8_t *min_ptr;                      
    uint8_t *max_ptr;                      
    uint8_t *bwp_base;                     
    uint8_t bwp_width; 
    uint64_t sa_max_value;
    uint8_t *lastptr;
    
    /**
     * @brief Construct a new Container object.
     */
    Container();

    /**
     * @brief Destroy the Container object.
     */
    ~Container();
};

}

#endif