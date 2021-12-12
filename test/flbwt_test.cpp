#include <gtest/gtest.h>
#include "flbwt.hpp"

TEST(flbwt_test, extract_LMS_strings_1)
{
    const uint8_t *T = (uint8_t *)"mmississiippii$";
    uint64_t n = 15;
    LMS_strings *lms = extract_LMS_strings(T, n);
    uint8_t EXPECTED_COUNT = 3;
    EXPECT_EQ(EXPECTED_COUNT, lms->count);
    free(lms);
}