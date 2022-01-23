#include <gtest/gtest.h>
#include "utility.hpp"

TEST(utility_test, log2_64_1)
{
    const uint64_t x = 65536;
    const uint8_t EXPECTED_VALUE = 16;
    EXPECT_EQ(EXPECTED_VALUE, flbwt::log2_64(x));
}

TEST(utility_test, log2_64_2)
{
    const uint64_t x = 0xffffffffffffffff;
    const uint8_t EXPECTED_VALUE = 63;
    EXPECT_EQ(EXPECTED_VALUE, flbwt::log2_64(x));
}

TEST(utility_test, position_of_ms_bit_1)
{
    const uint64_t x = 7;
    const uint8_t EXPECTED_VALUE = 3;
    EXPECT_EQ(EXPECTED_VALUE, flbwt::position_of_msb(x));
}

TEST(utility_test, position_of_ms_bit_2)
{
    const uint64_t x = 0xffffffffffffffff;
    const uint8_t EXPECTED_VALUE = 64;
    EXPECT_EQ(EXPECTED_VALUE, flbwt::position_of_msb(x));
}

