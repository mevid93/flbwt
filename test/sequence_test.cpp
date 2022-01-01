#include <gtest/gtest.h>
#include "sequence.hpp"
#include "utility.hpp"

TEST(sequence_test, construct_sequence_1)
{
    int n = 15;
    std::unique_ptr<flbwt::Sequence> sequence(new flbwt::Sequence(n, 100));
    uint8_t EXPECTED_VALUE = 2 * n + 2;
    EXPECT_EQ(EXPECTED_VALUE, sequence->upper_stream_length);
}

TEST(sequence_test, construct_sequence_2)
{
    int n = 1000;
    std::unique_ptr<flbwt::Sequence> sequence(new flbwt::Sequence(n, 2000));
    uint32_t EXPECTED_VALUE = 3000;
    EXPECT_EQ(EXPECTED_VALUE, sequence->lower_stream_length);
}

TEST(sequence_test, store_integer_1)
{
    int n = 15;
    std::unique_ptr<flbwt::Sequence> sequence(new flbwt::Sequence(n, 1600));
    int index = 7;
    int value = 1584;
    sequence->store_integer(index, value);
    uint8_t EXPECTED_VALUE = 1;
    uint8_t i = 7 + 6;
    EXPECT_EQ(EXPECTED_VALUE, (*sequence->upper_stream)[i]);
}

TEST(sequence_test, store_integer_2)
{
    int n = 3;
    std::unique_ptr<flbwt::Sequence> sequence(new flbwt::Sequence(n, 334));
    sequence->store_integer(3, 333);
    sequence->store_integer(1, 111);
    sequence->store_integer(2, 222);
    EXPECT_EQ(1, (*sequence->upper_stream)[1 + 0]);
    EXPECT_EQ(1, (*sequence->upper_stream)[2 + 0]);
    EXPECT_EQ(1, (*sequence->upper_stream)[3 + 1]);
    uint64_t lbits = 8;
    EXPECT_EQ(111U, sequence->lower_stream->get_int(1, lbits));
    EXPECT_EQ(222U, sequence->lower_stream->get_int(9, lbits));
    EXPECT_EQ(77U, sequence->lower_stream->get_int(17, lbits));
}

TEST(sequence_test, get_integer_1)
{
    int n = 15;
    std::unique_ptr<flbwt::Sequence> sequence(new flbwt::Sequence(n, 600));
    int index = 1;
    int value = 541;
    sequence->store_integer(index, value);
    uint64_t EXPECTED_VALUE = 541;
    EXPECT_EQ(EXPECTED_VALUE, sequence->get_integer(1));
}

TEST(sequence_test, get_integer_2)
{
    int n = 10;
    uint64_t MAX_SUPPORTED_U = 0xffffffffffffffff / 256;
    std::unique_ptr<flbwt::Sequence> sequence(new flbwt::Sequence(n, MAX_SUPPORTED_U));
    sequence->store_integer(3, 333);
    sequence->store_integer(1, 111);
    sequence->store_integer(2, 222);
    sequence->store_integer(4, 44444);
    sequence->store_integer(6, MAX_SUPPORTED_U - 100000);
    sequence->store_integer(5, MAX_SUPPORTED_U - 1000000);
    sequence->store_integer(8, MAX_SUPPORTED_U - 1000);
    sequence->store_integer(9, MAX_SUPPORTED_U - 10);
    sequence->store_integer(10, MAX_SUPPORTED_U - 1);
    sequence->store_integer(7, MAX_SUPPORTED_U - 10000);
    EXPECT_EQ(333U, sequence->get_integer(3));
    EXPECT_EQ(111U, sequence->get_integer(1));
    EXPECT_EQ(222U, sequence->get_integer(2));
    EXPECT_EQ(44444U, sequence->get_integer(4));
    EXPECT_EQ(MAX_SUPPORTED_U - 100000, sequence->get_integer(6));
    EXPECT_EQ(MAX_SUPPORTED_U - 1000000, sequence->get_integer(5));
    EXPECT_EQ(MAX_SUPPORTED_U - 1000, sequence->get_integer(8));
    EXPECT_EQ(MAX_SUPPORTED_U - 10, sequence->get_integer(9));
    EXPECT_EQ(MAX_SUPPORTED_U - 1, sequence->get_integer(10));
    EXPECT_EQ(MAX_SUPPORTED_U - 10000, sequence->get_integer(7));
}