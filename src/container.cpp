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
    std::fill_n(this->c_substr_counts, 256, 0);
}

flbwt::Container::~Container()
{
    delete this->hashtable;
    this->hashtable = NULL;
}
