#include <gtest/gtest.h>
#include "flbwt.hpp"

TEST(flbwt_test, extract_LMS_strings_1)
{
    const uint8_t *T = (uint8_t *)"mmississiippii$";
    const uint64_t n = 15;
    const uint8_t k = 5;
    flbwt::Container *container = flbwt::extract_LMS_strings(T, n, k);
    EXPECT_EQ(3U, container->num_of_substrings);
    EXPECT_EQ(2U, container->num_of_unique_substrings);
    delete container;
}

TEST(flbwt_test, extract_LMS_strings_2)
{
    const uint8_t *T = (uint8_t *)"issasissssssssssssssssssssssssssssssssssssssssssssssssssssssssss$";
    const uint64_t n = 66;
    const uint8_t k = 4;
    flbwt::Container *container = flbwt::extract_LMS_strings(T, n, k);
    EXPECT_EQ(2U, container->num_of_substrings);
    EXPECT_EQ(2U, container->num_of_unique_substrings);
    delete container;
}