#include "hashtable.hpp"
#include "utility.hpp"

flbwt::HashTable::HashTable(const uint64_t hash_table_size, const uint64_t n, const uint8_t k)
{
    this->HTSIZE = hash_table_size;
    this->HBSIZE = 512 * 8;
    this->rest = new uint64_t[this->HTSIZE];
    this->head = new uint64_t[this->HTSIZE];
    std::fill_n(this->rest, this->HTSIZE, 0);
    std::fill_n(this->head, this->HTSIZE, 0);
    this->buf = new sdsl::bit_vector(0, 0);
    this->collisions = 0;
    this->LSI_BITS = flbwt::position_of_msb(n);
    this->LSL_BITS = this->LSI_BITS;
    this->SS_LIMIT = flbwt::log2_64(n) / flbwt::log2_64(k);
    this->SS_BITS = flbwt::position_of_msb(this->SS_LIMIT);
    this->NAME_BITS = this->LSI_BITS;
    this->SW_BITS = 8;
}

uint8_t flbwt::HashTable::insert_string(const uint8_t *T, const uint64_t m, const uint64_t p)
{
    uint64_t i;                              // index of the substring
    uint64_t pp;                             // previous substring starting index
    uint64_t l;                              // length of substring under comparison
    bool short_string = m <= this->SS_LIMIT; // determine type of the string

    // start by calculating the hash
    uint64_t h = this->hash_function(T, m, p);

    // If a string with same hash exists, keep scanning until
    // free position is reached, or duplicate is detected.
    uint64_t q = this->head[h];
    while (q != 0)
    {
        pp = q;
        l = this->get_length(q);

        if (l == 0)
            break; // no more strings found

        if (l == 1) // end of block --> continue from next point
        {
            q = this->get_pointer(q);
            this->collisions++;
            continue;
        }

        if (l != m) // not the same string if length differs
        {
            q = this->skip_string(q);
            this->collisions++;
            continue;
        }

        if (l <= this->SS_LIMIT)
        {
            // compare the substring (to be insterted) with the short substring of length l
            for (i = 0U; i < m; i++)
            {
                if (T[p + i] != this->buf->get_int(q + this->SW_BITS + 1 + this->SS_BITS + i * 8, 8))
                    break;
            }
        }
        else
        {
            uint64_t p2 = this->get_position(q);

            // compare the substring (to be insterted) with the long substring of length l
            for (i = 0U; i < m; i++)
            {
                if (T[p + i] != T[p2 + i])
                    break;
            }
        }

        if (i == m) // duplicate string
            return 0;

        q = this->skip_string(q);
        this->collisions++;
    }

    // resize the bit-vector if needed
    bool was_resized = false;
    uint64_t bufsize = this->buf->bit_size();
    uint64_t q2 = bufsize + 1;
    uint8_t block_end_bits = this->SW_BITS + 1 + this->SS_BITS + 64;
    if (short_string && (this->SW_BITS + 1U + this->SS_BITS + 8U * m + this->NAME_BITS + block_end_bits) >= this->rest[h])
    {
        this->buf->bit_resize(bufsize + this->HBSIZE + this->SW_BITS + 1U + this->SS_BITS + 8 * m + this->NAME_BITS);
        this->rest[h] = this->HBSIZE + this->SW_BITS + 1U + this->SS_BITS + 8U * m + this->NAME_BITS;
        was_resized = true;
    }
    else if (!short_string && (this->SW_BITS + 1U + this->LSI_BITS + this->LSL_BITS + this->NAME_BITS + block_end_bits) >= this->rest[h])
    {
        this->buf->bit_resize(bufsize + this->HBSIZE + this->SW_BITS + 1U + this->LSI_BITS + this->LSL_BITS + this->NAME_BITS);
        this->rest[h] = this->HBSIZE + this->SW_BITS + 1U + this->LSI_BITS + this->LSL_BITS + this->NAME_BITS;
        was_resized = true;
    }

    if (was_resized)
    {
        if (q == 0)
        {
            this->head[h] = q2; // first block
        }
        else
        {
            this->set_length(pp, 1);
            this->set_pointer(pp, q2);
        }

        q = q2;
    }

    if (short_string)
    {
        this->set_length(q, m);                         // length
        this->buf->set_int(q, T[p] + 1, this->SW_BITS); // sentinel
        (*this->buf)[q + this->SW_BITS] = 0;            // type
        for (i = 0; i < m; i++)
        {
            this->buf->set_int(q + this->SW_BITS + 1 + this->SS_BITS + i * 8, T[p + i], 8); // content
        }
        q += this->SW_BITS + 1 + this->SS_BITS + m * 8 + this->NAME_BITS; 
        this->rest[h] -= this->SW_BITS + 1 + this->SS_BITS + m * 8 + this->NAME_BITS;
    }
    else
    {
        this->set_length(q, m);                         // length
        this->buf->set_int(q, T[p] + 1, this->SW_BITS); // sentinel
        (*this->buf)[q + this->SW_BITS] = 1;            // type
        this->set_position(q, p);                       // position
        q += this->SW_BITS + 1 + this->LSI_BITS + this->LSI_BITS + this->NAME_BITS;
        this->rest[h] -= this->SW_BITS + 1 + this->LSI_BITS + this->LSL_BITS + this->NAME_BITS;
    }

    this->buf->set_int(q, 0, 64); // make sure that next bytes are zero --> get_length will return zero

    return 1; // new string added
}

uint64_t flbwt::HashTable::hash_function(const uint8_t *T, const uint64_t m, const uint64_t p)
{
    uint64_t x = 0;

    for (uint64_t i = 0; i < m; i++)
    {
        x *= 101;
        x += T[p + i];
    }

    return x %= this->HTSIZE;
}

uint64_t flbwt::HashTable::get_length(uint64_t idx)
{
    bool short_string = (*this->buf)[idx + this->SW_BITS] == 0;

    if (short_string)
        return this->buf->get_int(idx + this->SW_BITS + 1, this->SS_BITS);

    return this->buf->get_int(idx + this->SW_BITS + 1 + this->LSI_BITS, this->LSL_BITS);
}

uint64_t flbwt::HashTable::get_pointer(uint64_t idx)
{
    return this->buf->get_int(idx + this->SW_BITS + 1 + this->SS_BITS, 64);
}

uint64_t flbwt::HashTable::skip_string(uint64_t idx)
{
    uint64_t length = this->get_length(idx);

    if (length <= this->SS_LIMIT)
    {
        return idx + this->SW_BITS + 1 + this->SS_BITS + length * 8 + this->NAME_BITS;
    }

    return idx + this->SW_BITS + 1 + this->LSI_BITS + this->LSL_BITS + this->NAME_BITS;
}

void flbwt::HashTable::set_length(uint64_t idx, uint64_t length)
{
    if (length <= this->SS_LIMIT)
    {
        (*this->buf)[idx + 8] = 0;
        this->buf->set_int(idx + this->SW_BITS + 1, length, this->SS_BITS);
        return;
    }

    (*this->buf)[idx + 8] = 1;
    this->buf->set_int(idx + this->SW_BITS + 1 + this->LSI_BITS, length, this->LSL_BITS);
}

void flbwt::HashTable::set_pointer(uint64_t idx, uint64_t p)
{
    this->buf->set_int(idx + this->SW_BITS + 1 + this->SS_BITS, p, 64);
}

uint64_t flbwt::HashTable::get_position(uint64_t idx)
{
    return this->buf->get_int(idx + this->SW_BITS + 1, this->LSI_BITS);
}

void flbwt::HashTable::set_position(uint64_t idx, uint64_t p)
{
    this->buf->set_int(idx + this->SW_BITS + 1, p, this->LSI_BITS);
}

void flbwt::HashTable::set_name(uint64_t idx, uint64_t p)
{
    if ((*this->buf)[idx + this->SW_BITS] == 0)
    {
        uint8_t m = this->get_length(idx);
        this->buf->set_int(idx + this->SW_BITS + 1 + this->SS_BITS + 8 * m, p, this->NAME_BITS);
        return;
    }

    this->buf->set_int(idx + this->SW_BITS + 1 + this->LSI_BITS + this->LSL_BITS, p, this->NAME_BITS);
}

uint64_t flbwt::HashTable::get_name(uint64_t idx)
{
    if ((*this->buf)[idx + this->SW_BITS] == 0)
    {
        uint8_t m = this->get_length(idx);
        return this->buf->get_int(idx + this->SW_BITS + 1 + this->SS_BITS + 8 * m, this->NAME_BITS);
    }

    return this->buf->get_int(idx + this->SW_BITS + 1 + this->LSI_BITS + this->LSL_BITS, this->NAME_BITS);
}

uint8_t flbwt::HashTable::get_nth_character(const uint8_t *T, uint64_t idx, uint64_t n)
{
    if ((*this->buf)[idx + this->SW_BITS] == 0)
    {
        return this->buf->get_int(idx + this->SW_BITS + 1 + this->SS_BITS + n * 8, 8);
    }

    uint64_t p = this->get_position(idx);
    return T[p + n];
}

flbwt::HashTable::~HashTable()
{
    delete[] this->rest;
    this->rest = NULL;
    delete[] this->head;
    this->head = NULL;
    delete this->buf;
    this->buf = NULL;
}