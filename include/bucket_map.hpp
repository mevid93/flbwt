#ifndef FLBWT_BUCKET_MAP_HPP
#define FLBWT_BUCKET_MAP_HPP

#include <stdint.h>
#include <unordered_map>
#include "bucket.hpp"

namespace flbwt
{

/**
 * @brief HashMap like datastructure that stores buckets.
 * This is wrapper class.
 */
class BucketMap
{
public:
    /**
     * @brief Construct a new BucketMap object.
     * 
     * @param bitcounts number of bits needed for each c-bucket.
     */
    BucketMap(uint64_t bitcounts[]);

    /**
     * @brief Get the all Buckets.
     * 
     * @return std::unordered_map<uint8_t, flbwt::Bucket*>* 
     */
    std::unordered_map<uint8_t, flbwt::Bucket*> *getAllBuckets();

    /**
     * @brief Get the total number of substrings stored in buckets.
     * 
     * @return uint64_t number of substrings.
     */
    uint64_t getTotalNumberOfSubstrings();

    /**
     * @brief Insert new substring to c-bucket.
     */
    void insert_substring(const uint8_t *T, const uint64_t n, const uint8_t k, const uint64_t p, const uint64_t l);

    /**
     * @brief Destroy the Bucket object.
     */
    ~BucketMap();

private:
    std::unordered_map<uint8_t, flbwt::Bucket*> *buckets;
    uint64_t num_of_substrings;
};

}

#endif