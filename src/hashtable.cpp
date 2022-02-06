#include <iostream>
#include "hashtable.hpp"
#include "utility.hpp"

flbwt::HashTable::HashTable(const uint64_t hash_table_size, const uint64_t n)
{
    this->HTSIZE = hash_table_size;
    this->HBSIZE = 512;
    this->rest = new uint64_t[this->HTSIZE];
    this->head = new uint64_t[this->HTSIZE];

    for (int64_t i = this->HTSIZE; --i;)
    {
        this->rest[i] = 0;
        this->head[i] = 0;
    }

    this->buf = NULL;
    this->bufsize = 0;
    this->LENGTH_X_BYTES = 1;
    this->SENTINEL_CHAR_BYTES = 1;

    // there can be maximum of n/2 substrings (names)
    uint64_t bits = flbwt::position_of_msb(n / 2 + 1);
    this->NAME_BYTES = bits / 8;
    if (bits % 8 != 0)
        this->NAME_BYTES++;
}

uint8_t flbwt::HashTable::insert_string(const uint64_t &m, uint8_t *p)
{
    uint64_t i;
    uint64_t pp;
    uint64_t l;
    uint8_t *r;
    uint8_t *r2;
    uint8_t l2;
    uint64_t q2;

    uint64_t h = this->hash_function(m, p);

    // If string with hash exists, scan until free position (or duplicate) is found
    uint64_t q = this->head[h];
    while (q != 0)
    {
        pp = q;
        r = &this->buf[q];
        l = flbwt::HashTable::get_length(r);

        // No more strings found
        if (l == 0)
        {
            r = &this->buf[pp];
            break;
        }

        // End of block --> continue from next block
        if (l == 2)
        {
            q = flbwt::HashTable::get_pointer(r);
            continue;
        }

        // Not the same string if length differs
        if (l != m)
        {
            q += *r; // bytes
            q += 2;  // x bytes and sentinel
            q += l + this->NAME_BYTES;
            continue;
        }

        // Compare the substring (to be insterted) with the existing substring
        l2 = *r;
        for (i = 0U; i < m; i++)
        {
            if (p[i] != this->buf[q + this->SENTINEL_CHAR_BYTES + this->LENGTH_X_BYTES + l2 + i])
                break;
        }

        // Duplicate string
        if (i == m)
            return 0;

        q += *r; // bytes
        q += 2;  // x bytes and sentinel
        q += l + this->NAME_BYTES;
    }

    // Resize the memory pool, if the new string cannot be fitted in it
    uint8_t length_bytes = flbwt::HashTable::lenlen(m);
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
            flbwt::HashTable::set_pointer(r2, q2);
        }

        r = &this->buf[q2];
    }

    flbwt::HashTable::set_length(r, length_bytes, m);
    r += 1 + length_bytes;
    *r++ = p[0] + 1; // sentinel
    for (uint64_t i = 0; i < m; i++)
        *r++ = p[i];

    // jump over name bytes
    r += this->NAME_BYTES;

    // clear the following fields
    for (uint8_t i = 0; i < 12; i++)
        *r++ = 0;

    this->rest[h] -= space_required;

    return 1;
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
        l = flbwt::HashTable::get_length(r);

        if (l == 0)
        {
            break; // not found
        }

        if (l == 2) // end of block --> continue from next point
        {
            q = flbwt::HashTable::get_pointer(r);
            continue;
        }

        if (l != m) // not the same string if length differs
        {
            q += *r; // bytes
            q += 2;  // x bytes and sentinel
            q += l + this->NAME_BYTES;
            continue;
        }

        // compare the substring (to be insterted) with the existing substring
        l2 = *r;
        for (i = 0U; i < m; i++)
        {
            if (p[i] != this->buf[q + this->SENTINEL_CHAR_BYTES + this->LENGTH_X_BYTES + l2 + i])
                break;
        }

        if (i == m) // duplicate string
            return this->get_name(r);

        q += *r; // bytes
        q += 2;  // x bytes and sentinel
        q += l + this->NAME_BYTES;
    }

    throw std::runtime_error("hashtable->find_name() failed: Substring was not found");
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

// static function members (generally, static functions are faster)

uint64_t flbwt::HashTable::hash_function(const uint64_t &m, uint8_t *p)
{
    uint64_t x = 0;

    for (int64_t i = (int64_t)m; i--;)
    {
        x *= 101;
        x += *p++;
    }

    return x % this->HTSIZE;
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

uint8_t flbwt::HashTable::lenlen(uint64_t m)
{
    uint8_t w = 1;
    m >>= 8;

    while (m != 0)
    {
        w++;
        m >>= 8;
    }

    return w;
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

void flbwt::HashTable::set_length(uint8_t *p, uint8_t length_bytes, uint64_t length)
{
    *p = length_bytes;
    p += length_bytes; 
    
    while (length != 0)
    {
        *p-- = length & 0xff;
        length >>= 8;
    }
}

uint8_t *flbwt::HashTable::get_first_character_pointer(uint8_t *p)
{
    uint8_t bytes = *p++;
    p += bytes;
    p++; // skip sentinel
    return p;
}