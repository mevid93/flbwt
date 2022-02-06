#include <gtest/gtest.h>
#include "hashtable.hpp"

TEST(hashtable_test, construct_hashtable_1)
{
    const uint64_t n = 15;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    EXPECT_EQ(100U, hashtable->HTSIZE);
    EXPECT_EQ(0U, hashtable->head[0]);
    EXPECT_EQ(0U, hashtable->rest[99]);
    EXPECT_EQ(NULL, hashtable->buf);
    EXPECT_EQ(0U, hashtable->bufsize);
    delete hashtable;
}

TEST(hashtable_test, hash_function_1)
{
    uint8_t *T = (uint8_t *)"mmississiippii$";
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, 15);
    const uint64_t h1 = hashtable->hash_function(4, &T[2]);
    const uint64_t h2 = hashtable->hash_function(4, &T[5]);
    const uint64_t h3 = hashtable->hash_function(7, &T[8]);
    EXPECT_EQ(40U, h1);
    EXPECT_EQ(80U, h3);
    EXPECT_EQ(h1, h2);
}

TEST(hashtable_test, get_length_1)
{
    const uint64_t n = 16;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t *)malloc(100 * sizeof(uint8_t));
    hashtable->bufsize = 100;
    hashtable->buf[0] = 3;
    hashtable->buf[1] = 123;                                               // 01111011
    hashtable->buf[2] = 123;                                               // 01111011
    hashtable->buf[3] = 123;                                               // 01111011
    EXPECT_EQ(8092539U, flbwt::HashTable::get_length(&hashtable->buf[0])); // 01111011 01111011 01111011
    delete hashtable;
}

TEST(hashtable_test, get_length_2)
{
    const uint64_t n = 16;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t *)malloc(100 * sizeof(uint8_t));
    hashtable->bufsize = 100;
    hashtable->buf[0] = 8;
    hashtable->buf[1] = 0xff;
    hashtable->buf[2] = 0xff;
    hashtable->buf[3] = 0xff;
    hashtable->buf[4] = 0xff;
    hashtable->buf[5] = 0xff;
    hashtable->buf[6] = 0xff;
    hashtable->buf[7] = 0xff;
    hashtable->buf[8] = 0xff;
    EXPECT_EQ(0xffffffffffffffff, flbwt::HashTable::get_length(&hashtable->buf[0]));
    delete hashtable;
}

TEST(hashtable_test, set_length_1)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t *)malloc(100 * sizeof(uint8_t));
    hashtable->bufsize = 100;
    flbwt::HashTable::set_length(&hashtable->buf[0], 3, 0xfeffef);
    EXPECT_EQ(3U, hashtable->buf[0]);
    EXPECT_EQ(0xfe, hashtable->buf[1]);
    EXPECT_EQ(0xff, hashtable->buf[2]);
    EXPECT_EQ(0xef, hashtable->buf[3]);
    delete hashtable;
}

TEST(hashtable_test, set_length_2)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t *)malloc(100 * sizeof(uint8_t));
    hashtable->bufsize = 100;
    flbwt::HashTable::set_length(&hashtable->buf[0], 3, 0xfe0000);
    EXPECT_EQ(3U, hashtable->buf[0]);
    EXPECT_EQ(0xfe, hashtable->buf[1]);
    EXPECT_EQ(0x00, hashtable->buf[2]);
    EXPECT_EQ(0x00, hashtable->buf[3]);
    delete hashtable;
}

TEST(hashtable_test, get_pointer_1)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t *)malloc(0xff * sizeof(uint8_t));
    hashtable->bufsize = 0xff;
    hashtable->buf[0] = 1;
    hashtable->buf[1] = 2; // length 2 --> pointer
    hashtable->buf[2] = 1; // 0x01
    hashtable->buf[3] = 2; // 0x02
    hashtable->buf[4] = 3; // 0x03
    hashtable->buf[5] = 4; // 0x04
    hashtable->buf[6] = 5; // 0x05
    hashtable->buf[7] = 6; // 0x06
    hashtable->buf[8] = 7; // 0x07
    hashtable->buf[9] = 8; // 0x08
    EXPECT_EQ(72623859790382856U, flbwt::HashTable::get_pointer(&hashtable->buf[0]));
    delete hashtable;
}

TEST(hashtable_test, set_pointer_1)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t *)malloc(0xff * sizeof(uint8_t));
    hashtable->bufsize = 0xff;
    hashtable->set_pointer(&hashtable->buf[0], 72623859790382856U);
    EXPECT_EQ(1U, hashtable->buf[0]); // x bytes character
    EXPECT_EQ(2U, hashtable->buf[1]); // length
    EXPECT_EQ(1U, hashtable->buf[2]); // first part of the pointer
    EXPECT_EQ(2U, hashtable->buf[3]);
    EXPECT_EQ(3U, hashtable->buf[4]);
    EXPECT_EQ(4U, hashtable->buf[5]);
    EXPECT_EQ(5U, hashtable->buf[6]);
    EXPECT_EQ(6U, hashtable->buf[7]);
    EXPECT_EQ(7U, hashtable->buf[8]);
    EXPECT_EQ(8U, hashtable->buf[9]); // part of the pointer
    delete hashtable;
}

TEST(hashtable_test, insert_string_1)
{
    uint8_t *T = (uint8_t *)"mmississiippii$mmississiippii$mmississiippii$mmississiippii$mmississiippii$";
    const uint64_t n = 76;
    flbwt::HashTable *hashtable = new flbwt::HashTable(10, n);
    hashtable->HBSIZE = 100;
    uint8_t result;
    result = hashtable->insert_string(3, &T[0]);

    EXPECT_EQ(1U, result);
    result = hashtable->insert_string(4, &T[1]);
    EXPECT_EQ(1U, result);
    result = hashtable->insert_string(3, &T[2]);
    EXPECT_EQ(1U, result);
    result = hashtable->insert_string(4, &T[3]);
    EXPECT_EQ(1U, result);
    result = hashtable->insert_string(3, &T[4]);
    EXPECT_EQ(1U, result);
    result = hashtable->insert_string(4, &T[5]);
    EXPECT_EQ(1U, result);
    result = hashtable->insert_string(4, &T[5]); // insert same string again

    EXPECT_EQ(0U, result);
    EXPECT_EQ(430U, hashtable->bufsize);

    delete hashtable;
}

TEST(hashtable_test, lenlen)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    EXPECT_EQ(5, flbwt::HashTable::lenlen(0xaabbccddee));
    delete hashtable;
}

TEST(hashtable_test, get_name_1)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t *)malloc(0xff * sizeof(uint8_t));
    hashtable->bufsize = 0xff;
    hashtable->NAME_BYTES = 2;
    hashtable->buf[0] = 1;
    hashtable->buf[1] = 1;
    hashtable->buf[4] = 1; // 0x01
    hashtable->buf[5] = 2; // 0x02
    EXPECT_EQ(0x0102U, hashtable->get_name(&hashtable->buf[0]));
    delete hashtable;
}

TEST(hashtable_test, set_name_1)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t *)malloc(0xff * sizeof(uint8_t));
    hashtable->bufsize = 0xff;
    hashtable->buf[0] = 1; // x bytes length
    hashtable->buf[1] = 1; // actual length
    hashtable->set_name(&hashtable->buf[0], 0xdead);
    EXPECT_EQ(0xdeU, hashtable->buf[4]);
    EXPECT_EQ(0xadU, hashtable->buf[5]);
    delete hashtable;
}

TEST(hashtable_test, get_first_character_pointer_1)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t *)malloc(0xff * sizeof(uint8_t));
    hashtable->bufsize = 0xff;
    hashtable->buf[0] = 1; // x bytes length
    hashtable->buf[1] = 1; // actual length
    uint8_t *p = hashtable->get_first_character_pointer(&hashtable->buf[0]);
    EXPECT_EQ(&hashtable->buf[3], p);
    delete hashtable;
}
