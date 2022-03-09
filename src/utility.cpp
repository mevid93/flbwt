#include "utility.hpp"

uint64_t flbwt::CUR_MEM_USE = 0;
uint64_t flbwt::MAX_MEM_USE = 0;

void flbwt::increase_memory_allocation(uint64_t n)
{
    flbwt::CUR_MEM_USE += n;

    if (flbwt::CUR_MEM_USE > flbwt::MAX_MEM_USE)
        flbwt::MAX_MEM_USE = flbwt::CUR_MEM_USE;
}


void flbwt::decrease_memory_allocation(uint64_t n)
{
    flbwt::CUR_MEM_USE -= n;

    if (flbwt::CUR_MEM_USE < 0)
        flbwt::CUR_MEM_USE = 0;
}