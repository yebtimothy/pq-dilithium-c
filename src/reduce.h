#ifndef REDUCE_H
#define REDUCE_H

#include <stdint.h>
#include "params.h"

#define QINV 58728449   // -q^-1 mod 2^32
#define MONT 3871200    // 2^32 mod q

int32_t montgomery_reduce(int64_t a);
int32_t barrett_reduce(int32_t a);
int32_t reduce32(int32_t a);
int32_t csubq(int32_t a);
int32_t freeze32(int32_t a);

#endif
