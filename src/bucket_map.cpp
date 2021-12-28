#include "bucket_map.hpp"

flbwt::BucketMap::BucketMap(uint64_t bitcounts[])
{
    this->buckets = new std::unordered_map<uint8_t, flbwt::Bucket *>();
    for (uint16_t i = 0; i < 256; i++)
    {
        if (bitcounts[i] != 0)
        {
            this->buckets->insert({i, new flbwt::Bucket(i, bitcounts[i])});
        }
    }

    this->num_of_substrings = 0;
}

std::unordered_map<uint8_t, flbwt::Bucket *> *flbwt::BucketMap::getAllBuckets()
{
    return this->buckets;
}

uint64_t flbwt::BucketMap::getTotalNumberOfSubstrings()
{
    return this->num_of_substrings;
}

void flbwt::BucketMap::insert_substring(const uint8_t *T, const uint64_t n, const uint8_t k, const uint64_t p, const uint64_t l)
{
    (*this->buckets)[T[p]]->insert_substring(T, n, k, p, l);
    this->num_of_substrings++;
}

flbwt::BucketMap::~BucketMap()
{
    for (std::unordered_map<uint8_t, flbwt::Bucket *>::iterator it = this->buckets->begin(); it != this->buckets->end(); it++)
    {
        delete it->second;
    }

    delete this->buckets;
}

