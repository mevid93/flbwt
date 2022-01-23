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
    const uint64_t n = 15;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    const uint64_t h1 = hashtable->hash_function(4, &T[2]);
    const uint64_t h2 = hashtable->hash_function(4, &T[5]);
    const uint64_t h3 = hashtable->hash_function(7, &T[8]);
    EXPECT_EQ(40U, h1);
    EXPECT_EQ(80U, h3);
    EXPECT_EQ(h1, h2);
    delete hashtable;
}

TEST(hashtable_test, get_length_1)
{
    const uint64_t n = 16;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t*)malloc(100 * sizeof(uint8_t));
    hashtable->bufsize = 100;
    hashtable->buf[1] = 3;
    hashtable->buf[2] = 123;                                        // 01111011
    hashtable->buf[3] = 123;                                        // 01111011
    hashtable->buf[4] = 123;                                        // 01111011
    EXPECT_EQ(8092539U, hashtable->get_length(&hashtable->buf[0])); // 01111011 01111011 01111011
    delete hashtable;
}

TEST(hashtable_test, get_length_2)
{
    const uint64_t n = 16;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t*)malloc(100 * sizeof(uint8_t));
    hashtable->bufsize = 100;
    hashtable->buf[1] = 8;
    hashtable->buf[2] = 0xff;
    hashtable->buf[3] = 0xff;
    hashtable->buf[4] = 0xff;
    hashtable->buf[5] = 0xff;
    hashtable->buf[6] = 0xff;
    hashtable->buf[7] = 0xff;
    hashtable->buf[8] = 0xff;
    hashtable->buf[9] = 0xff;
    EXPECT_EQ(0xffffffffffffffff, hashtable->get_length(&hashtable->buf[0]));
    delete hashtable;
}

TEST(hashtable_test, set_length_1)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t*)malloc(100 * sizeof(uint8_t));
    hashtable->bufsize = 100;
    hashtable->set_length(&hashtable->buf[0], 0xfeffef);
    EXPECT_EQ(3U, hashtable->buf[1]);
    EXPECT_EQ(0xfe, hashtable->buf[2]);
    EXPECT_EQ(0xff, hashtable->buf[3]);
    EXPECT_EQ(0xef, hashtable->buf[4]);
    delete hashtable;
}

TEST(hashtable_test, set_length_2)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t*)malloc(100 * sizeof(uint8_t));
    hashtable->bufsize = 100;
    hashtable->set_length(&hashtable->buf[0], 0xfe0000);
    EXPECT_EQ(3U, hashtable->buf[1]);
    EXPECT_EQ(0xfe, hashtable->buf[2]);
    EXPECT_EQ(0x00, hashtable->buf[3]);
    EXPECT_EQ(0x00, hashtable->buf[4]);
    delete hashtable;
}

TEST(hashtable_test, string_info_length_1)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t*)malloc(100 * sizeof(uint8_t));
    hashtable->bufsize = 100;
    hashtable->set_length(&hashtable->buf[0], 0xff);
    uint64_t chars = hashtable->string_info_length(&hashtable->buf[0], 0xff);
    EXPECT_EQ(chars, 1U + 1U + 1U + 0xffU + hashtable->NAME_BYTES);
    delete hashtable;
}

TEST(hashtable_test, string_info_length_2)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t*)malloc(0xffff * sizeof(uint8_t));
    hashtable->bufsize = 0xffff;
    hashtable->set_length(&hashtable->buf[0], 0xffee);
    uint64_t chars = hashtable->string_info_length(&hashtable->buf[0], 0xffee);
    EXPECT_EQ(chars, 1U + 1U + 2U + 0xffeeU + hashtable->NAME_BYTES);
    delete hashtable;
}

TEST(hashtable_test, get_pointer_1)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t*)malloc(0xff * sizeof(uint8_t));
    hashtable->bufsize = 0xff;
    hashtable->buf[1] = 1;
    hashtable->buf[2] = 1;  // length 1 --> pointer
    hashtable->buf[3] = 1;  // 0x01
    hashtable->buf[4] = 2;  // 0x02
    hashtable->buf[5] = 3;  // 0x03
    hashtable->buf[6] = 4;  // 0x04
    hashtable->buf[7] = 5;  // 0x05
    hashtable->buf[8] = 6;  // 0x06
    hashtable->buf[9] = 7;  // 0x07
    hashtable->buf[10] = 8; // 0x08
    EXPECT_EQ(72623859790382856U, hashtable->get_pointer(&hashtable->buf[0]));
    delete hashtable;
}

TEST(hashtable_test, set_pointer_1)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t*)malloc(0xff * sizeof(uint8_t));
    hashtable->bufsize = 0xff;
    hashtable->set_pointer(&hashtable->buf[0], 72623859790382856U);
    EXPECT_EQ(1U, hashtable->buf[1]); // x bytes character
    EXPECT_EQ(2U, hashtable->buf[2]); // length
    EXPECT_EQ(1U, hashtable->buf[3]); // first part of the pointer
    EXPECT_EQ(2U, hashtable->buf[4]);
    EXPECT_EQ(3U, hashtable->buf[5]);
    EXPECT_EQ(4U, hashtable->buf[6]);
    EXPECT_EQ(5U, hashtable->buf[7]);
    EXPECT_EQ(6U, hashtable->buf[8]);
    EXPECT_EQ(7U, hashtable->buf[9]);
    EXPECT_EQ(8U, hashtable->buf[10]); // part of the pointer
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

TEST(hashtable_test, get_lenlen)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t*)malloc(0xff * sizeof(uint8_t));
    hashtable->bufsize = 0xff;
    hashtable->buf[1] = 3;
    EXPECT_EQ(3U, hashtable->get_lenlen(&hashtable->buf[0])); // x bytes character
    delete hashtable;
}

TEST(hashtable_test, calculate_lenlen)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    EXPECT_EQ(5, hashtable->calculate_lenlen(0xaabbccddee)); 
    delete hashtable;
}

TEST(hashtable_test, get_name_1)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t*)malloc(0xff * sizeof(uint8_t));
    hashtable->bufsize = 0xff;
    hashtable->NAME_BYTES = 2;
    hashtable->buf[1] = 1;
    hashtable->buf[2] = 1;
    hashtable->buf[4] = 1;  // 0x01
    hashtable->buf[5] = 2;  // 0x02
    EXPECT_EQ(0x0102U, hashtable->get_name(&hashtable->buf[0])); 
    delete hashtable;
}

TEST(hashtable_test, set_name_1)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t*)malloc(0xff * sizeof(uint8_t));
    hashtable->bufsize = 0xff;
    hashtable->buf[1] = 1;  // x bytes length
    hashtable->buf[2] = 1;  // actual length
    hashtable->set_name(&hashtable->buf[0], 0xdead);
    EXPECT_EQ(0xdeU, hashtable->buf[4]); 
    EXPECT_EQ(0xadU, hashtable->buf[5]);
    delete hashtable;
}

TEST(hashtable_test, get_first_character_pointer_1)
{
    const uint64_t n = 1000;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n);
    hashtable->buf = (uint8_t*)malloc(0xff * sizeof(uint8_t));
    hashtable->bufsize = 0xff;
    hashtable->buf[1] = 1;  // x bytes length
    hashtable->buf[2] = 1;  // actual length
    uint8_t *p = hashtable->get_first_character_pointer(&hashtable->buf[0]);
    EXPECT_EQ(&hashtable->buf[3], p); 
    delete hashtable;
}

/*




TEST(hashtable_test, find_name_1)
{
    const uint8_t *T = (uint8_t *)"mmississiippii$";
    const uint64_t n = 15;
    const uint8_t k = 5;
    flbwt::HashTable *hashtable = new flbwt::HashTable(100, n, k);
    hashtable->insert_string(T, 4, 2);
    uint64_t hash = hashtable->hash_function(T, 4, 2);
    uint64_t q = hashtable->head[hash];
    hashtable->set_name(q, 1);
    hashtable->insert_string(T, 7, 8);
    hash = hashtable->hash_function(T, 7, 8);
    q = hashtable->head[hash];
    hashtable->set_name(q, 2);
    EXPECT_EQ(1U, hashtable->find_name(T, 4, 2));
    EXPECT_EQ(2U, hashtable->find_name(T, 7, 8));
    EXPECT_ANY_THROW(hashtable->find_name(T, 3, 3));
    delete hashtable;
}*/