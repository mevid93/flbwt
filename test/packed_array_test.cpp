#include <gtest/gtest.h>
#include "packed_array.hpp"

TEST(packed_array_test, construct_packed_array_1)
{
    flbwt::PackedArray *pa = new flbwt::PackedArray(100, 16);
    EXPECT_EQ(100U, pa->n);
    EXPECT_EQ(16U, pa->w);
    delete pa;
}

TEST(packed_array_test, pa_set_1)
{
    flbwt::PackedArray *pa = new flbwt::PackedArray(100, 32);
    pa->pa_set(0, 33);
    EXPECT_EQ(33U, pa->b[0]);
    delete pa;
}

TEST(packed_array_test, pa_set_2)
{
    flbwt::PackedArray *pa = new flbwt::PackedArray(100, 8);
    pa->pa_set(0, 33);
    EXPECT_EQ(33U, (pa->b[0]) >> (32 - 8));
    delete pa;
}

TEST(packed_array_test, pa_get_1)
{
    flbwt::PackedArray *pa = new flbwt::PackedArray(100, 32);
    pa->pa_set(0, 33);
    EXPECT_EQ(33U, pa->pa_get(0));
    delete pa;
}

TEST(packed_array_test, pa_get_2)
{
    flbwt::PackedArray *pa = new flbwt::PackedArray(100, 16);
    pa->pa_set(0, 33);
    pa->pa_set(1, 32767);
    pa->pa_set(2, 6699);
    EXPECT_EQ(33U, pa->pa_get(0));
    EXPECT_EQ(32767U, pa->pa_get(1));
    EXPECT_EQ(6699U, pa->pa_get(2));
    delete pa;
}

TEST(packed_array_test, pa_get_3)
{
    flbwt::PackedArray *pa = new flbwt::PackedArray(100, 42);
    pa->pa_set(0, 33);
    pa->pa_set(1, 32767);
    pa->pa_set(2, 6699);
    EXPECT_EQ(33U, pa->pa_get(0));
    EXPECT_EQ(32767U, pa->pa_get(1));
    EXPECT_EQ(6699U, pa->pa_get(2));
    delete pa;
}