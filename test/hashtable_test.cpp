#include <gtest/gtest.h>
#include "hashtable.hpp"

TEST(hashtable_test, construct_hashtable_1)
{
    const uint64_t n = 15;
    const uint8_t k = 5;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    EXPECT_EQ(100U, hashtable->HTSIZE);
    EXPECT_EQ(4U, hashtable->LSI_BITS);
    EXPECT_EQ(4U, hashtable->LSL_BITS);
    EXPECT_EQ(1U, hashtable->SS_LIMIT);
    EXPECT_EQ(1U, hashtable->SS_BITS);
    EXPECT_EQ(0U, hashtable->head[0]);
    EXPECT_EQ(0U, hashtable->rest[99]);
    EXPECT_EQ(0U, hashtable->buf->bit_size());
    delete hashtable;
}

TEST(hashtable_test, hash_function_1)
{
    const uint8_t *T = (uint8_t *)"mmississiippii$";
    const uint64_t n = 15;
    const uint8_t k = 5;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    const uint64_t h1 = hashtable->hash_function(T, 4, 2);
    const uint64_t h2 = hashtable->hash_function(T, 4, 5);
    const uint64_t h3 = hashtable->hash_function(T, 7, 8);
    EXPECT_EQ(40U, h1);
    EXPECT_EQ(80U, h3);
    EXPECT_EQ(h1, h2);
    delete hashtable;
}

TEST(hashtable_test, get_length_1)
{
    const uint64_t n = 16;
    const uint8_t k = 2;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    hashtable->buf->bit_resize(100);
    hashtable->buf->set_int(9, 4, hashtable->SS_BITS);
    uint64_t length = hashtable->get_length(0);
    EXPECT_EQ(4U, length);
    delete hashtable;
}

TEST(hashtable_test, get_length_2)
{
    const uint64_t n = 16;
    const uint8_t k = 2;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    hashtable->buf->bit_resize(100);
    (*hashtable->buf)[8] = 1;
    hashtable->buf->set_int(8 + 1 + hashtable->LSI_BITS, 15, hashtable->LSL_BITS);
    uint64_t length = hashtable->get_length(0);
    EXPECT_EQ(15U, length);
    delete hashtable;
}

TEST(hashtable_test, get_pointer_1)
{
    const uint64_t n = 16;
    const uint8_t k = 2;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    hashtable->buf->bit_resize(100);
    hashtable->buf->set_int(8 + 1, 1, hashtable->SS_BITS);
    hashtable->buf->set_int(8 + 1 + hashtable->SS_BITS, 313, 64);
    uint64_t index = hashtable->get_pointer(0);
    EXPECT_EQ(313U, index);
    delete hashtable;
}

TEST(hashtable_test, skip_string_1)
{
    const uint64_t n = 16;
    const uint8_t k = 2;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    hashtable->buf->bit_resize(100);
    hashtable->buf->set_int(9, 4, hashtable->SS_BITS);
    uint64_t index = hashtable->skip_string(0);
    EXPECT_EQ(49U, index);
    delete hashtable;
}

TEST(hashtable_test, skip_string_2)
{
    const uint64_t n = 16;
    const uint8_t k = 2;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    hashtable->buf->bit_resize(100);
    (*hashtable->buf)[8] = 1;
    hashtable->buf->set_int(8 + 1 + hashtable->LSI_BITS, 15, hashtable->LSL_BITS);
    uint64_t index = hashtable->skip_string(0);
    EXPECT_EQ(24U, index);
    delete hashtable;
}

TEST(hashtable_test, set_length_1)
{
    const uint64_t n = 1000;
    const uint8_t k = 6;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    hashtable->buf->bit_resize(100);
    hashtable->set_length(0, 3);
    EXPECT_EQ(3U, hashtable->get_length(0));
    delete hashtable;
}

TEST(hashtable_test, set_length_2)
{
    const uint64_t n = 1000;
    const uint8_t k = 6;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    hashtable->buf->bit_resize(100);
    hashtable->set_length(0, 25);
    EXPECT_EQ(25U, hashtable->get_length(0));
    delete hashtable;
}

TEST(hashtable_test, set_pointer_1)
{
    const uint64_t n = 100;
    const uint8_t k = 2;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    hashtable->buf->bit_resize(100);
    hashtable->set_length(0, 1);
    hashtable->set_pointer(0, 66);
    EXPECT_EQ(66U, hashtable->get_pointer(0));
    delete hashtable;
}

TEST(hashtable_test, get_position_1)
{
    const uint64_t n = 1000;
    const uint8_t k = 6;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    hashtable->buf->bit_resize(100);
    (*hashtable->buf)[8] = 1;
    hashtable->buf->set_int(8 + 1, 70, hashtable->LSI_BITS);
    uint64_t p = hashtable->get_position(0);
    EXPECT_EQ(70U, p);
    delete hashtable;
}

TEST(hashtable_test, set_position_1)
{
    const uint64_t n = 1000;
    const uint8_t k = 6;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    hashtable->buf->bit_resize(100);
    (*hashtable->buf)[8] = 1;
    hashtable->set_position(0, 70);
    uint64_t p = hashtable->get_position(0);
    EXPECT_EQ(70U, p);
    delete hashtable;
}

TEST(hashtable_test, resize_1)
{
    const uint8_t *T = (uint8_t *)"mmississiippii$mmississiippii$mmississiippii$mmississiippii$mmississiippii$";
    const uint64_t n = 76;
    const uint8_t k = 5;
    flbwt::HashTable *hashtable = new flbwt::HashTable(10, n, k);
    hashtable->HBSIZE = 100;
    hashtable->insert_string(T, 2, 0);
    hashtable->insert_string(T, 4, 1);
    hashtable->insert_string(T, 2, 2);
    hashtable->insert_string(T, 4, 3);
    hashtable->insert_string(T, 2, 4);
    hashtable->insert_string(T, 4, 5);
    EXPECT_EQ(792U, hashtable->buf->bit_size());
    delete hashtable;
}

TEST(hashtable_test, get_name_1)
{
    const uint64_t n = 1000;
    const uint8_t k = 6;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    hashtable->buf->bit_resize(100);
    (*hashtable->buf)[8] = 1;
    hashtable->buf->set_int(8 + 1 + hashtable->LSI_BITS + hashtable->LSL_BITS, 42, hashtable->NAME_BITS);
    uint64_t name = hashtable->get_name(0);
    EXPECT_EQ(42U, name);
    delete hashtable;
}

TEST(hashtable_test, set_name_1)
{
    const uint64_t n = 1000;
    const uint8_t k = 6;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    hashtable->buf->bit_resize(100);
    (*hashtable->buf)[8] = 0;
    hashtable->set_name(0, 44);
    uint64_t name = hashtable->get_name(0);
    EXPECT_EQ(44U, name);
    delete hashtable;
}