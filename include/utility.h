#ifndef FLBWT_UTILITY_H
#define FLBWT_UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Calculate the alphabet size of the input string.
 * 
 * @param T input string
 * @param n input string lentgth
 * @return uint8_t alphabet size
 */
uint8_t calculate_alphabet_size(const uint8_t *T, const uint64_t n);

#ifdef __cplusplus
}
#endif

#endif