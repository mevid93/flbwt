#include <gtest/gtest.h>
#include "utility.h"

TEST(utility_test, calculate_alphabet_size_1)
{
    const uint8_t *T = (uint8_t *)"abcdefghij";
    const uint64_t n = 10;
    const uint8_t EXPECTED_VALUE = 10;
    EXPECT_EQ(EXPECTED_VALUE, calculate_alphabet_size(T, n));
}

TEST(utility_test, calculate_alphabet_size_2)
{
    const uint8_t *T = (uint8_t *)"ababababab";
    const uint64_t n = 10;
    const uint8_t EXPECTED_VALUE = 2;
    EXPECT_EQ(EXPECTED_VALUE, calculate_alphabet_size(T, n));
}