#include <algorithm>
#include <stddef.h>
#include "container.hpp"

flbwt::Container::Container()
{
    this->hashtable = NULL;
    this->num_of_substrings = 0;
    this->k = 0;
    this->n = 0;
    this->num_of_unique_substrings = 0;
    std::fill_n(this->M, 256 + 2, 0);
    std::fill_n(this->M2, 256 + 2, 0);
    std::fill_n(this->M3, 256 + 2, 0);
    std::fill_n(this->C, 256 + 2, 0);
    std::fill_n(this->C2, 256 + 2, 0);
    std::fill_n(this->NL, 256+ 2, 0);
}

flbwt::Container::~Container()
{
    delete this->hashtable;
    this->hashtable = NULL;
}
