#ifndef FLBWT_QUEUE_HPP
#define FLBWT_QUEUE_HPP

#include <stdint.h>
#include "packed_array.hpp"

namespace flbwt
{

typedef struct qblock
{
    qblock *prev;
    qblock *next;
    flbwt::PackedArray *b;
};

#define QSIZ 1024

/**
 * @brief Queue class.
 */
class Queue
{
public:
    /**
     * @brief Construct a new Queue object.
     */
    Queue(uint8_t w);

    /**
     * @brief Destroy the Queue object.
     */
    ~Queue();

private:
    uint64_t n; // number of elements
    uint8_t w;  // width of elements in bits
    flbwt::qblock *sb;
    flbwt::qblock *eb;
    uint64_t s_ofs; // 0 <= s_ofs
    uint64_t e_ofs; // e_ofs < QSIZ
};

}

#endif