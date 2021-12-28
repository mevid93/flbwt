#include <gtest/gtest.h>
#include "bucket_map.hpp"
#include "flbwt.hpp"

TEST(bucket_map_test, construct_bucket_map_1)
{
    const uint8_t *T = (uint8_t *)"mmississiippii$";
    const uint64_t n = 15;
    const uint8_t k = 5;
    uint64_t *counts = flbwt::count_bits_for_buckets(T, n, k);
    flbwt::BucketMap *bucket_map = new flbwt::BucketMap(counts);
    EXPECT_EQ(1U, bucket_map->getAllBuckets()->size());
    delete bucket_map;
    delete[] counts;
}

TEST(bucket_map_test, get_total_number_of_substrings_1)
{
    const uint8_t *T = (uint8_t *)"mmississiippii$";
    const uint64_t n = 15;
    const uint8_t k = 5;
    uint64_t *counts = flbwt::count_bits_for_buckets(T, n, k);
    flbwt::BucketMap *bucket_map = new flbwt::BucketMap(counts);
    EXPECT_EQ(0UL, bucket_map->getTotalNumberOfSubstrings());
    delete bucket_map;
    delete[] counts;
}

TEST(bucket_map_test, insert_substring_1)
{
    const uint8_t *T = (uint8_t *)"mmississiippii$";
    const uint64_t n = 15;
    const uint8_t k = 5;
    uint64_t *counts = flbwt::count_bits_for_buckets(T, n, k);
    flbwt::BucketMap *bucket_map = new flbwt::BucketMap(counts);
    EXPECT_EQ(0UL, bucket_map->getTotalNumberOfSubstrings());
    EXPECT_EQ(0UL, bucket_map->getTotalNumberOfSubstrings());
    bucket_map->insert_substring(T, n, k, 2, 4);
    EXPECT_EQ(1UL, bucket_map->getTotalNumberOfSubstrings());
    delete bucket_map;
    delete[] counts;
}