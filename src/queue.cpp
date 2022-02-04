#include <stddef.h>
#include <stdlib.h>
#include "queue.hpp"
#include "utility.hpp"

flbwt::Queue::Queue(uint8_t w)
{
    this->n = 0;
    this->w = w;
    this->sb = NULL;
    this->eb = NULL;
    this->s_ofs = 0;
    this->e_ofs = QSIZ - 1;
    this->max_value = flbwt::max_integer(w);
}

flbwt::Queue::~Queue()
{
    qblock *qb, *q;

    qb = this->sb;

    while (qb != NULL)
    {
        q = qb->next;

        delete qb->b;
        free(qb);

        qb = q;
    }
}

void flbwt::Queue::enqueue(uint64_t x)
{
    qblock *qb;

    if (this->e_ofs == QSIZ - 1)
    { // current block is full
        qb = (qblock *)malloc(sizeof(qblock));
        qb->b = new PackedArray(QSIZ, this->max_value, false);

        if (this->eb == NULL)
        { // no blocks
            this->sb = this->eb = qb;
            this->s_ofs = 0;
            qb->prev = qb->next = NULL;
        }
        else
        {
            this->eb->next = qb;    // add after the current last block
            qb->prev = this->eb;
            qb->next = NULL;
            this->eb = qb;
        }

        this->e_ofs = -1;
    }

    this->eb->b->set_value(++this->e_ofs, x);
    this->n++;
}

void flbwt::Queue::enqueue_l(uint64_t x)
{
    qblock *qb;

    if (this->s_ofs == 0)
    { // current block is full
        qb = (qblock *)malloc(sizeof(qblock));
        qb->b = new PackedArray(QSIZ, this->max_value, false);

        if (this->sb == NULL)
        { // no block exists
            this->sb = this->eb = qb;
            this->e_ofs = QSIZ - 1;
            qb->prev = qb->next = NULL;
        }
        else
        {
            this->sb->prev = qb; // add before the current first block
            qb->next = this->sb;
            qb->prev = NULL;
            this->sb = qb;
        }

        this->s_ofs = QSIZ;
    }

    this->sb->b->set_value(--this->s_ofs, x);
    this->n++;
}

int64_t flbwt::Queue::dequeue()
{
    qblock *qb;

    int64_t x = this->sb->b->get_value(this->s_ofs++);

    if (this->s_ofs == QSIZ)
    { // current block is empty
        qb = this->sb;
        this->sb = qb->next;
        delete qb->b;
        free(qb);

        if (this->sb == NULL)
        { // the block is gone
            this->eb = NULL;
            this->e_ofs = QSIZ - 1;
            this->s_ofs = 0;
        }
        else
        {
            this->sb->prev = NULL;
            this->s_ofs = 0;
        }
    }

    this->n--;
    return x;
}

bool flbwt::Queue::is_empty()
{
    return this->n == 0;
}