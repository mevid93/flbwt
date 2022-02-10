#include <iostream>
#include "induce.hpp"
#include "queue.hpp"

#ifndef TYPE_S
#define TYPE_S 0
#define TYPE_L 1
#define TYPE_LMS 2
#endif

flbwt::BWT_result *flbwt::induce_bwt(flbwt::PackedArray *SA, flbwt::Container *container)
{
    uint8_t bwp_w = container->bwp_width;
    uint8_t *bwp_base = container->bwp_base;

    // define variable to hold all queues
    flbwt::Queue *Q[3][256 + 2];

    // initialize queues
    for (uint16_t i = 0; i <= 256 + 1; i++)
    {
        Q[TYPE_LMS][i] = new flbwt::Queue(bwp_w);
        Q[TYPE_L][i] = new flbwt::Queue(bwp_w);
        Q[TYPE_S][i] = new flbwt::Queue(bwp_w);
    }

    int64_t i;
    int64_t c;
    uint8_t *q;

    for (i = container->num_of_substrings; i >= 0; i--)
    {
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
    }

    // delete SA --> big performance boost (extra heap becomes available for next allocation)
    delete SA;

    // allocate memory for bwt
    uint8_t *BWT = new uint8_t[container->n + 1];
    std::fill_n(BWT, container->n + 1, 0);

    int64_t cc = 0;
    for (i = 0; i <= 256 + 1; i++)
    {
        container->M3[i] = container->M2[i] = cc;
        cc += container->M[i];
    }

    // C2 is the beginning of TYPE_S bucket
    for (i = 0; i <= 256 + 1; i++)
        container->C2[i] = container->M2[i] + container->NL[i];

    // M2: Copy destination address, default is the beginning of the bucket
    // C2: The address of the stack that temporarily stores bwt, the initial value is the position of the S bucket.
    // M3: Bucket position (doesn't change)

    int64_t c1;
    int64_t c2;
    i = 0;
    q = bwp_base + Q[TYPE_LMS][i]->dequeue();
    c1 = q[-1];
    c2 = -1;
    BWT[0] = c1;
    Q[TYPE_L][c1 + 1]->enqueue((q - 1) - bwp_base);
    BWT[container->M2[c1 + 1]++] = q[-2];
    BWT[container->C2[c2 + 1]++] = c1;

    int64_t m;
    int t;
    uint64_t last;

    for (c = 1; c <= 256 + 1; c++)
    {
        for (t = 1; t <= 2; t++)
        {                         // process TYPE_L and TYPE LMS in sequence
            m = container->M3[c]; // head of L bucket

            if (t == TYPE_LMS)
                m += container->M[c] - container->C[c]; // head of LMS bucket

            while (!Q[t][c]->is_empty())
            {
                q = bwp_base + Q[t][c]->dequeue();

                if (q == container->lastptr)
                {
                    last = m;
                }
                else
                {
                    c1 = q[-1];

                    if (c1 >= c - 1)
                    { // TYPE_L
                        Q[TYPE_L][c1 + 1]->enqueue((q - 1) - bwp_base);
                        BWT[container->M2[c1 + 1]++] = q[-2];

                        if (t == TYPE_LMS)
                        {
                            BWT[container->C2[c]++] = c1;
                        }
                    }
                    else
                    {
                        Q[TYPE_S][c]->enqueue_l(q - bwp_base);
                    }
                }

                m++;
            }
        }
    }

    for (c = 0; c <= 256 + 1; c++)
    {
        delete Q[TYPE_LMS][c];
        delete Q[TYPE_L][c];
    }

    for (c = 0; c <= 256 + 1; c++)
        Q[TYPE_L][c] = new flbwt::Queue(bwp_w);

    container->M2[0] = 0;
    for (c = 1; c <= 256 + 1; c++)
        container->M2[c] = container->M2[c - 1] + container->M[c];

    int64_t c0;
    for (c = 256 + 1; c >= 0; c--)
    {
        for (t = 1; t >= 0; t--)
        {
            while(!Q[t][c]->is_empty())
            {
                q = bwp_base + Q[t][c]->dequeue();
                c1 = q[-1];

                if (c1 <= c - 1)
                {
                    Q[TYPE_L][c1 + 1]->enqueue((q - 1) - bwp_base);

                    if (q - 1 == container->lastptr)
                    {
                        last = container->M2[c1 + 1]--;
                    }
                    else
                    {
                        c0 = q[-2];
                        BWT[container->M2[c1 + 1]--] = (c0 <= c1) ? c0 : BWT[--container->C2[c1 + 1]];
                    }
                }
            }
        }
    }

    for (uint16_t i = 0; i <= 256 + 1; i++)
    {
        delete Q[TYPE_L][i];
        delete Q[TYPE_S][i];
    }

    // return the result bwt
    BWT_result *bwt_result = (BWT_result *)malloc(sizeof(BWT_result));
    bwt_result->last = last;
    bwt_result->BWT = BWT;

    return bwt_result;
}