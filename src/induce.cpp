#include <iostream>
#include "induce.hpp"
#include "queue.hpp"

#ifndef TYPE_S
#define TYPE_S 0
#define TYPE_L 1
#define TYPE_LMS 2
#endif

uint8_t *flbwt::induce_bwt(flbwt::PackedArray *SA, flbwt::Container *container)
{
    uint8_t bwp_w = container->bwp_width;
    uint8_t *bwp_base = container->bwp_base;

    // define variable to hold all queues
    flbwt::Queue *Q[3][256 + 2];

    // initialize queues
    for (uint16_t i = 0; i < 256; i++)
    {
        Q[TYPE_LMS][i] = new flbwt::Queue(bwp_w);
        Q[TYPE_L][i] = new flbwt::Queue(bwp_w);
        Q[TYPE_S][i] = new flbwt::Queue(bwp_w);
    }

    int64_t i;
    int64_t c;

    for (i = container->num_of_substrings; i >= 0; i--)
    {
        uint8_t *q;
        q = SA->get_value(i) + bwp_base;
        
        if (i == 0)
        {
            c = -1;
        }
        else
        {
            c = q[0];
        }

        Q[TYPE_LMS][c + 1]->enqueue_l(q - bwp_base);

        if (i % 1024 == 0)
        {
            // reallocate --> release space that is no more needed
            SA->reallocate(i);
        }
    }

    // allocate memory for bwt
    uint8_t *BWT = new uint8_t[container->n + 1];

    // TODO!!!!

    for (uint16_t i = 0; i < 256; i++)
    {
        delete Q[TYPE_LMS][i];
        delete Q[TYPE_L][i];
        delete Q[TYPE_S][i];
    }

    // return the result bwt
    return BWT;
}