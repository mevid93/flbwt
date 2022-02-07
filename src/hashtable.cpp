#include <iostream>
#include <algorithm>
#include "hashtable.hpp"
#include "utility.hpp"

flbwt::HashTable::HashTable(const uint64_t hash_table_size, const uint64_t n)
{
    this->HTSIZE = hash_table_size;
    this->HBSIZE = 512;
    this->rest = new uint64_t[this->HTSIZE];
    this->head = new uint64_t[this->HTSIZE];
    std::fill_n(this->rest, this->HTSIZE, 0);
    std::fill_n(this->head, this->HTSIZE, 0);
    this->buf = NULL;
    this->bufsize = 0;
    this->collisions = 0;
    this->LENGTH_X_BYTES = 1;
    this->SENTINEL_CHAR_BYTES = 1;

    // there can be maximum of n/2 substrings (names)
    uint64_t max_name = n / 2 + 1;
    uint64_t bits = flbwt::position_of_msb(max_name);
    this->NAME_BYTES = bits / 8;
    if (bits % 8 != 0)
        this->NAME_BYTES++;
}

uint8_t flbwt::HashTable::insert_string(const uint64_t m, uint8_t *p)
{
    uint64_t i;  // index of the substring
    uint64_t pp; // previous substring starting index
    uint64_t l;  // length of substring under comparison
    uint8_t *r = NULL;
    uint8_t *r2 = NULL;
    uint8_t l2;
    uint64_t q2;

    // start by calculating the hash
    uint64_t h = this->hash_function(m, p);

    // If a string with same hash exists, keep scanning until
    // free position is reached, or duplicate is detected.
    uint64_t q = this->head[h];
    while (q != 0)
    {
        pp = q;
        r = &this->buf[q];
        l = this->get_length(r);

        if (l == 0)
        {
            r = &this->buf[pp];
            break; // no more strings found
        }

        if (l == 2) // end of block --> continue from next point
        {
            q = this->get_pointer(r);
            this->collisions++;
            continue;
        }

        if (l != m) // not the same string if length differs
        {
            q += this->string_info_length(r, l);
            this->collisions++;
            continue;
        }

        // compare the substring (to be insterted) with the existing substring
        l2 = this->get_lenlen(r);
        for (i = 0U; i < m; i++)
        {
            if (p[i] != this->buf[q + this->SENTINEL_CHAR_BYTES + this->LENGTH_X_BYTES + l2 + i])
                break;
        }

        if (i == m) // duplicate string
            return 0;

        q += this->string_info_length(r, l);
        this->collisions++;
    }

    // resize the bit-vector if needed
    uint8_t length_bytes = this->calculate_lenlen(m);
    uint64_t space_required = this->SENTINEL_CHAR_BYTES + this->LENGTH_X_BYTES + length_bytes + m + this->NAME_BYTES;
    if ((int64_t)space_required >= (int64_t)this->rest[h] - 12)
    {
        r2 = (uint8_t *)realloc(this->buf, this->bufsize + this->HBSIZE + space_required);

        if (r2 != this->buf)
            this->buf = r2;

        q2 = this->bufsize + 1;
        this->bufsize += this->HBSIZE + space_required;
        this->rest[h] = this->HBSIZE + space_required;

        if (q == 0)
        {
            // first block
            this->head[h] = q2;
        }
        else
        {
            r2 = &this->buf[pp];
            this->set_pointer(r2, q2);
        }

        r = &this->buf[q2];
    }

    this->set_length(r, m);
    r += 1 + length_bytes;
    *r++ = p[0] + 1; // sentinel
    for (uint64_t i = 0; i < m; i++)
    {
        *r++ = p[i];
    }

    // jump over name bytes
    r += this->NAME_BYTES;

    // reset the following fields
    for (uint8_t i = 0; i < 12; i++)
        *r++ = 0;

    this->rest[h] -= space_required;

    return 1; // new string added
}

uint64_t flbwt::HashTable::hash_function(const uint64_t m, uint8_t *p)
{
    uint64_t x = 0;

    for (uint64_t i = 0; i < m; i++)
    {
        x *= 101;

        x += *p++;
    }

    x %= this->HTSIZE;

    return x;
}

uint64_t flbwt::HashTable::get_length(uint8_t *p)
{
    uint8_t bytes = *p++; // number of bytes used to store the length
    if (bytes == 0)
        return 0;

    uint64_t length = *p++;
    for (uint8_t i = 1; i < bytes; i++)
    {
        length = (length << 8) | *p++;
    }
    return length;
}

void flbwt::HashTable::set_length(uint8_t *p, uint64_t length)
{
    uint8_t *r;
    uint8_t value = (length & 0xff00000000000000) >> 56;
    r = p;
    *r = 8;
    p++;

    while (value == 0)
    {
        length <<= 8;
        value = (length & 0xff00000000000000) >> 56;
        (*r)--;
    }

    while (length != 0)
    {
        value = (length & 0xff00000000000000) >> 56;
        *p++ = value;
        length <<= 8;
    }
}

uint64_t flbwt::HashTable::string_info_length(uint8_t *p, uint64_t length)
{
    uint64_t total_length = 0;

    uint8_t bytes = *p++; // x bytes character
    total_length += 1;
    total_length++; // sentinel
    total_length += bytes + length + this->NAME_BYTES;

    return total_length;
}

uint64_t flbwt::HashTable::get_pointer(uint8_t *p)
{
    uint64_t x = 0;
    p += 2; // skip length information

    for (uint8_t i = 0; i < 8; i++)
    {
        x <<= 8;
        x = x | *p++;
    }

    return x;
}

void flbwt::HashTable::set_pointer(uint8_t *p, uint64_t pointer)
{
    *p++ = 1; // x bytes character
    *p++ = 2; // length
    p += 7;
    uint8_t count = 0;
    while (count < 8)
    {
        *p-- = pointer & 0xff;
        pointer >>= 8;
        count++;
    }
}

uint8_t flbwt::HashTable::get_lenlen(uint8_t *p)
{
    return *p;
}

uint8_t flbwt::HashTable::calculate_lenlen(uint64_t m)
{
    uint8_t bytes = 8;
    uint8_t value = (m & 0xff00000000000000) >> 56;

    while (value == 0)
    {
        m <<= 8;
        value = (m & 0xff00000000000000) >> 56;
        bytes--;
    }

    return bytes;
}

uint64_t flbwt::HashTable::get_name(uint8_t *p)
{
    // get length
    uint64_t length = this->get_length(p);

    // move pointer to the beginning of name area
    uint8_t bytes = *p++;
    p += bytes + length;
    p++; // sentinel
    uint64_t name = 0;

    for (uint8_t i = 0; i < this->NAME_BYTES; i++)
    {
        name <<= 8;
        name = name | *p++;
    }

    return name;
}

void flbwt::HashTable::set_name(uint8_t *p, uint64_t name)
{
    // get length
    uint64_t length = this->get_length(p);

    // move pointer to beginning of name area
    uint8_t bytes = *p++; // skip x bytes character
    p += bytes + length;
    p++; //skip sentinel

    p += this->NAME_BYTES - 1;
    for (uint8_t i = 0; i < this->NAME_BYTES; i++)
    {
        *p-- = name & 0xff;
        name >>= 8;
    }
}

uint8_t *flbwt::HashTable::get_first_character_pointer(uint8_t *p)
{
    uint8_t bytes = *p++;
    p += bytes;
    p++; // skip sentinel
    return p;
}

uint64_t flbwt::HashTable::find_name(uint64_t m, uint8_t *p)
{
    uint64_t i; // index of the substring
    uint64_t l; // length of substring under comparison
    uint8_t *r;
    uint8_t l2;

    // start by calculating the hash
    uint64_t h = this->hash_function(m, p);

    // If a string with same hash exists, keep scanning until
    // free position is reached, or duplicate is detected.
    uint64_t q = this->head[h];
    while (q != 0)
    {
        r = &this->buf[q];
        l = this->get_length(r);

        if (l == 0)
        {
            break; // not found
        }

        if (l == 2) // end of block --> continue from next point
        {
            q = this->get_pointer(r);
            continue;
        }

        if (l != m) // not the same string if length differs
        {
            q += this->string_info_length(r, l);
            continue;
        }

        // compare the substring (to be insterted) with the existing substring
        l2 = this->get_lenlen(r);
        for (i = 0U; i < m; i++)
        {
            if (p[i] != this->buf[q + this->SENTINEL_CHAR_BYTES + this->LENGTH_X_BYTES + l2 + i])
                break;
        }

        if (i == m) // duplicate string
            return this->get_name(r);

        q += this->string_info_length(r, l);
    }

    throw std::runtime_error("hashtable->find_name() failed: Substring was not found");
}

flbwt::HashTable::~HashTable()
{
    delete[] this->rest;
    this->rest = NULL;
    delete[] this->head;
    this->head = NULL;
    if (this->buf != NULL)
        free(this->buf);
    this->buf = NULL;
}