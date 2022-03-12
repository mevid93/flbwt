#ifndef FLBWT_QUEUE_HPP
#define FLBWT_QUEUE_HPP

#include <stdint.h>
#include "packed_array.hpp"

namespace flbwt
{

struct qblock
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
     * @brief Insert value to the end of the queue.
     * 
     * @param x value
     */
    void enqueue(uint64_t x);

    /**
     * @brief Insert value to the beginning of the queue.
     * 
     * @param x value
     */
    void enqueue_l(uint64_t x);

    /**
     * @brief Remove value from the beginning of the queue.
     * 
     * @return int64_t value that was removed
     */
    int64_t dequeue();

    /**
     * @brief Check wheter queue is empty or not.
     * 
     * @return true queue is empty
     * @return false queue is not empty
     */
    bool is_empty();

    /**
     * @brief Destroy the Queue object.
     */
    ~Queue();

private:
    uint64_t n; // number of elements
    uint8_t w;  // width of elements in bits
    flbwt::qblock *sb;
    flbwt::qblock *eb;
    int64_t s_ofs; // 0 <= s_ofs
    int64_t e_ofs; // e_ofs < QSIZ
    uint64_t max_value;  // max unsigned integer that can be stored
};

}

#endif