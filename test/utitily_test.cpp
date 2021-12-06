#include <gtest/gtest.h>
#include "utility.hpp"

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

TEST(utility_test, log2_64_1)
{
    const uint64_t x = 65536;
    const uint8_t EXPECTED_VALUE = 16;
    EXPECT_EQ(EXPECTED_VALUE, log2_64(x));
}

TEST(utility_test, log2_64_2)
{
    const uint64_t x = 0xffffffffffffffff;
    const uint8_t EXPECTED_VALUE = 63;
    EXPECT_EQ(EXPECTED_VALUE, log2_64(x));
}

TEST(utility_test, position_of_ms_bit_1)
{
    const uint64_t x = 7;
    const uint8_t EXPECTED_VALUE = 3;
    EXPECT_EQ(EXPECTED_VALUE, position_of_msb(x));
}

TEST(utility_test, position_of_ms_bit_2)
{
    const uint64_t x = 0xffffffffffffffff;
    const uint8_t EXPECTED_VALUE = 64;
    EXPECT_EQ(EXPECTED_VALUE, position_of_msb(x));
}

