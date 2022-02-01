#include "induce.hpp"
#include "queue.hpp"

#ifndef TYPE_S
#define TYPE_S 0
#define TYPE_L 1
#define TYPE_LMS 2
#endif

void flbwt::induce_bwt(flbwt::PackedArray *SA, flbwt::Container *container)
{
    uint8_t bwp_w = container->bwp_width;
    uint8_t *bwp_base = container->bwp_base;

    // define variable to hold all queues
    flbwt::Queue *Q[3][256];

    // initialize queues
    for (uint16_t i = 0; i < 256; i++)
    {
        Q[TYPE_LMS][i] = new flbwt::Queue(bwp_w);
        Q[TYPE_L][i] = new flbwt::Queue(bwp_w);
        Q[TYPE_S][i] = new flbwt::Queue(bwp_w);
    }

    int64_t j;
    int64_t i;
    uint64_t c;

    j = container->num_of_substrings + 2 + 1;

    for (i = container->num_of_substrings + 2; i >= 0; i--)
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

        // enqueue... TODO
    }

}