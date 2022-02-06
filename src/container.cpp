#include "container.hpp"
#include <stddef.h>

flbwt::Container::Container(uint64_t n)
{
    this->hashtable = NULL;
    this->num_of_substrings = 0;
    this->n = n;
    this->num_of_unique_substrings = 0;

    for(int i = 256 + 2; i--;)
    {
        this->M[i] = 0;
        this->M2[i] = 0;
        this->M3[i] = 0;
        this->C[i] = 0;
        this->C2[i] = 0;
        this->NL[i] = 0;
    }
}

flbwt::Container::~Container()
{
    delete this->hashtable;
    this->hashtable = NULL;
}
