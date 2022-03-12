#include <gtest/gtest.h>
#include "sais40bit.hpp"

TEST(sais40bit_test, set_40bit_value_1)
{
    uint32_t *L = new uint32_t[5];
    int8_t *U = new int8_t[5];

    flbwt::set_40bit_value(L, U, 0, 1);
    flbwt::set_40bit_value(L, U, 1, -1);

    EXPECT_EQ(1, flbwt::get_40bit_value(L, U, 0));
    EXPECT_EQ(-1, flbwt::get_40bit_value(L, U, 1));

    delete[] L;
    delete[] U;
}

TEST(sais40bit_test, set_40bit_value_2)
{
    uint32_t *L = new uint32_t[5];
    int8_t *U = new int8_t[5];

    flbwt::set_40bit_value(L, U, 0, 3);
    flbwt::set_40bit_value(L, U, 1, -3);

    EXPECT_EQ(3, flbwt::get_40bit_value(L, U, 0));
    EXPECT_EQ(-3, flbwt::get_40bit_value(L, U, 1));

    delete[] L;
    delete[] U;
}

TEST(sais40bit_test, set_40bit_value_3)
{
    uint32_t *L = new uint32_t[5];
    int8_t *U = new int8_t[5];

    flbwt::set_40bit_value(L, U, 0, 7);
    flbwt::set_40bit_value(L, U, 1, -7);

    EXPECT_EQ(7, flbwt::get_40bit_value(L, U, 0));
    EXPECT_EQ(-7, flbwt::get_40bit_value(L, U, 1));

    delete[] L;
    delete[] U;
}

TEST(sais40bit_test, set_40bit_value_4)
{
    uint32_t *L = new uint32_t[5];
    int8_t *U = new int8_t[5];

    flbwt::set_40bit_value(L, U, 0, 15);
    flbwt::set_40bit_value(L, U, 1, -15);

    EXPECT_EQ(15, flbwt::get_40bit_value(L, U, 0));
    EXPECT_EQ(-15, flbwt::get_40bit_value(L, U, 1));

    delete[] L;
    delete[] U;
}

TEST(sais40bit_test, set_40bit_value_5)
{
    uint32_t *L = new uint32_t[5];
    int8_t *U = new int8_t[5];

    flbwt::set_40bit_value(L, U, 0, 0xffffffff);
    flbwt::set_40bit_value(L, U, 1, -0xffffffff);

    EXPECT_EQ(0xffffffff, flbwt::get_40bit_value(L, U, 0));
    EXPECT_EQ(-0xffffffff, flbwt::get_40bit_value(L, U, 1));

    delete[] L;
    delete[] U;
}

TEST(sais40bit_test, set_40bit_value_6)
{
    uint32_t *L = new uint32_t[5];
    int8_t *U = new int8_t[5];

    flbwt::set_40bit_value(L, U, 0, 5047149423);
    flbwt::set_40bit_value(L, U, 1, -5047149423);

    EXPECT_EQ(5047149423, flbwt::get_40bit_value(L, U, 0));
    EXPECT_EQ(-5047149423, flbwt::get_40bit_value(L, U, 1));

    delete[] L;
    delete[] U;
}

TEST(sais40bit_test, set_40bit_value_7)
{
    uint32_t *L = new uint32_t[5];
    int8_t *U = new int8_t[5];

    flbwt::set_40bit_value(L, U, 0, ~5047149423);
    flbwt::set_40bit_value(L, U, 1, ~(-5047149423));

    EXPECT_EQ(~5047149423, flbwt::get_40bit_value(L, U, 0));
    EXPECT_EQ(~(-5047149423), flbwt::get_40bit_value(L, U, 1));

    delete[] L;
    delete[] U;
}