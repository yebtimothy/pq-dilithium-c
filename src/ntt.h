#ifndef NTT_H
#define NTT_H

#include <stdint.h>

extern const int32_t zetas[256];

void ntt(int32_t a[256]);
void invntt_tomont(int32_t a[256]);

#endif
