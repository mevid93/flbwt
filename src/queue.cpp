#include <stddef.h>
#include <stdlib.h>
#include "queue.hpp"
#include "utility.hpp"

flbwt::Queue::Queue(uint8_t w)
{
    this->n = 0;
    this->w = 0;
    this->sb = NULL;
    this->eb = NULL;
    this->s_ofs = 0;
    this->e_ofs = QSIZ - 1;
    this->max_value = flbwt::max_integer(w);
}

flbwt::Queue::~Queue()
{
    qblock *tmp;

    while(this->sb != NULL)
    {
        tmp = this->sb;
        this->sb = this->sb->next;
        
        if (tmp->b != NULL)
            delete tmp->b;
        
        free(tmp);
    }
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