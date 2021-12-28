#include <gtest/gtest.h>
#include "flbwt.hpp"

TEST(flbwt_test, count_bits_for_buckets_1)
{
    const uint8_t *T = (uint8_t *)"mmississiippii$";
    uint64_t n = 15;
    uint64_t *counts = flbwt::count_bits_for_buckets(T, n, 5);
    EXPECT_EQ(27UL, counts['i']);
    delete counts;
}

TEST(flbwt_test, count_bits_for_buckets_2)
{
    const uint8_t *T = (uint8_t *)"i$";
    uint64_t n = 2;
    uint64_t *counts = flbwt::count_bits_for_buckets(T, n, 2);
    uint64_t sum = 0;
    for(int i = 0; i < 256; i++)
    {
        sum += counts[i];
    }
    EXPECT_EQ(0UL, sum);
    delete counts;
}

TEST(flbwt_test, count_bits_for_buckets_3)
{
    const uint8_t *T = (uint8_t *)"issasissssssssssssssssssssssssssssssssssssssssssssssssssssssssss$";
    uint64_t n = 66;
    uint64_t *counts = flbwt::count_bits_for_buckets(T, n, 4);
    EXPECT_EQ(27UL, counts['a']);
    delete counts;
}

TEST(flbwt_test, extract_LMS_strings_1)
{
    const uint8_t *T = (uint8_t *)"mmississiippii$";
    uint64_t n = 15;
    flbwt::BucketMap *bucket_map = flbwt::extract_LMS_strings(T, n, 5);
    uint8_t EXPECTED_COUNT = 1;
    EXPECT_EQ(EXPECTED_COUNT, bucket_map->getAllBuckets()->size());
    EXPECT_EQ(3UL, (*bucket_map->getAllBuckets())['i']->strings);
    delete bucket_map;
}

TEST(flbwt_test, extract_LMS_strings_2)
{
    const uint8_t *T = (uint8_t *)"issasissssssssssssssssssssssssssssssssssssssssssssssssssssssssss$";
    uint64_t n = 66;
    flbwt::BucketMap *bucket_map = flbwt::extract_LMS_strings(T, n, 4);
    uint8_t EXPECTED_COUNT = 2;
    EXPECT_EQ(EXPECTED_COUNT, bucket_map->getAllBuckets()->size());
    EXPECT_EQ(1UL, (*bucket_map->getAllBuckets())['a']->strings);
    delete bucket_map;
}