#include <gtest/gtest.h>
#include "bucket.hpp"

TEST(bucket_test, construct_bucket_1)
{
    const flbwt::Bucket *bucket = new flbwt::Bucket('a', 100);
    EXPECT_EQ('a', bucket->c);
    EXPECT_EQ(100UL, bucket->bits);
    EXPECT_EQ(0UL, bucket->strings);
    delete bucket;
}

TEST(bucket_test, insert_substring_1)
{
    const uint8_t *T = (uint8_t *)"mmississiippii$";
    uint64_t n = 15;
    flbwt::Bucket *bucket = new flbwt::Bucket('i', 27);
    bucket->insert_substring(T, n, 5, 2, 4);
    sdsl::bit_vector EXPECTED(27);
    // 1 0100 0010 0000...
    EXPECTED[0] = 1;    
    EXPECTED[2] = 1;
    EXPECTED[7] = 1;
    EXPECT_EQ(EXPECTED, *bucket->b);
    delete bucket;
}

TEST(bucket_test, insert_substring_2)
{
    const uint8_t *T = (uint8_t *)"issasissssssssssssssssssssssssssssssssssssssssssssssssssssssssss$";
    uint64_t n = 66;
    flbwt::Bucket *bucket = new flbwt::Bucket('i', 100);
    bucket->insert_substring(T, n, 4, 3, 3);
    sdsl::bit_vector EXPECTED(100);
    // 0 11 10000110 11001110 10010110 ...   
    EXPECTED[1] = 1;
    EXPECTED[2] = 1;
    EXPECTED[3] = 1;
    EXPECTED[8] = 1;
    EXPECTED[9] = 1;
    EXPECTED[11] = 1;
    EXPECTED[12] = 1;
    EXPECTED[15] = 1;
    EXPECTED[16] = 1;
    EXPECTED[17] = 1;
    EXPECTED[19] = 1;
    EXPECTED[22] = 1;
    EXPECTED[24] = 1;
    EXPECTED[25] = 1;
    EXPECT_EQ(EXPECTED, *bucket->b);
    EXPECT_EQ(1UL, bucket->strings);
    delete bucket;
}