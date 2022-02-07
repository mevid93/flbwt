#include <gtest/gtest.h>
#include "flbwt.hpp"

TEST(flbwt_test, extract_LMS_strings_1)
{
    uint8_t *T = (uint8_t *)"mmississiippii$";
    const uint64_t n = 15;
    flbwt::Container *container = flbwt::extract_LMS_strings(T, n);
    EXPECT_EQ(3U, container->num_of_substrings);
    EXPECT_EQ(2U, container->num_of_unique_substrings);
    EXPECT_EQ(2U, container->head_string_end);
    delete container;
}

TEST(flbwt_test, extract_LMS_strings_2)
{
    uint8_t *T = (uint8_t *)"issasissssssssssssssssssssssssssssssssssssssssssssssssssssssssss$";
    const uint64_t n = 65;
    flbwt::Container *container = flbwt::extract_LMS_strings(T, n);
    EXPECT_EQ(2U, container->num_of_substrings);
    EXPECT_EQ(2U, container->num_of_unique_substrings);
    delete container;
}

TEST(flbwt_test, bwt_string_1)
{
    uint8_t *T = (uint8_t *)"mmississiippii$";
    const uint64_t n = 15;
    flbwt::BWT_result *result = flbwt::bwt_string(T, n, false);
    EXPECT_EQ(9U, result->last);
    EXPECT_EQ('$', result->BWT[0]);
    EXPECT_EQ('i', result->BWT[1]);
    EXPECT_EQ('i', result->BWT[2]);
    EXPECT_EQ('p', result->BWT[3]);
    EXPECT_EQ('s', result->BWT[4]);
    EXPECT_EQ('i', result->BWT[5]);
    EXPECT_EQ('s', result->BWT[6]);
    EXPECT_EQ('m', result->BWT[7]);
    EXPECT_EQ('m', result->BWT[8]);
    EXPECT_EQ('n', result->BWT[9]);
    EXPECT_EQ('p', result->BWT[10]);
    EXPECT_EQ('i', result->BWT[11]);
    EXPECT_EQ('s', result->BWT[12]);
    EXPECT_EQ('s', result->BWT[13]);
    EXPECT_EQ('i', result->BWT[14]);
    EXPECT_EQ('i', result->BWT[15]);
    delete[] result->BWT;
    free(result);
}
