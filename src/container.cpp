#include "container.hpp"

flbwt::Container::Container()
{
    this->b = NULL;
    this->sls = NULL;
    this->rs = NULL;
    this->hashtable = NULL;
    this->sequence = NULL;
    this->num_of_substrings = 0;
    this->k = 0;
    this->n = 0;
    this->num_of_unique_substrings = 0;
    std::fill_n(this->c_substr_counts, 256, 0);
}

flbwt::Container::~Container()
{
    delete this->rs;
    this->rs = NULL;
    delete this->sls;
    this->sls = NULL;
    delete this->b;
    this->b = NULL;
    delete this->hashtable;
    this->hashtable = NULL;
    delete this->sequence;
    this->sequence = NULL;
}
