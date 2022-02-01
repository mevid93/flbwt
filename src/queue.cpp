#include <stddef.h>
#include "queue.hpp"

flbwt::Queue::Queue(uint8_t w)
{
    this->n = 0;
    this->w = 0;
    this->sb = NULL;
    this->eb = NULL;
    this->s_ofs = 0;
    this->e_ofs = QSIZ - 1;
}

flbwt::Queue::~Queue()
{

}