#ifndef FLBWT_CONTAINER_HPP
#define FLBWT_CONTAINER_HPP

#include <stdint.h>
#include "hashtable.hpp"
#include "sequence.hpp"

namespace flbwt
{

/**
 * @brief Container class for storing the results of LMS-extraction.
 */
class Container
{
public:
    sdsl::bit_vector *b;                   // bit array indicating indexes of S* substrings
    sdsl::select_support_mcl<1> *sls;      // select support for bit array b
    sdsl::rank_support_v5<1> *rs;          // rank support for bit array b
    flbwt::Sequence *sequence;             // lemma 1 data structure
    flbwt::HashTable *hashtable;           // hashtable for storing substrings
    uint64_t c_substr_counts[256];         // number of S* substrings starting with character C 
    uint64_t num_of_substrings;            // number of all substrings
    uint64_t num_of_unique_substrings;     // number of unique substrings
    uint64_t n;                            // length of the input string
    uint64_t k;                            // alphabet size
    uint64_t head_string_end;              // index of the last character of the head string T[0...p]
    
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