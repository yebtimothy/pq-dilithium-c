#include <stdint.h>
#include "params.h"
#include "reduce.h"

/*************************************************
* Name:        montgomery_reduce
* Description: Montgomery reduction; given a 64-bit integer a, computes
*              an integer r equivalent to a * 2^-32 (mod q) such that
*              -q < r < q.
* Arguments:   - int64_t a: input integer to be reduced; must be in range
*                           [-q*2^31, q*2^31]
* Returns:     integer r in range [-q, q]
**************************************************/
int32_t montgomery_reduce(int64_t a) {
  int32_t t;

  t = (int32_t)a * QINV;
  t = (a - (int64_t)t * DILITHIUM_Q) >> 32;
  return t;
}

/*************************************************
* Name:        barrett_reduce
* Description: Barrett reduction; given a 32-bit integer a, computes
*              an integer r equivalent to a (mod q) such that
*              -q < r < q.
* Arguments:   - int32_t a: input integer to be reduced; must be in range
*                           [-2^31, 2^31 - 1]
* Returns:     integer r in range [-q, q]
**************************************************/
int32_t barrett_reduce(int32_t a) {
  int32_t t;
  const int32_t v = ((1U << 26) + DILITHIUM_Q/2) / DILITHIUM_Q; // 8

  t = ((int64_t)a * v + (1U << 25)) >> 26;
  t *= DILITHIUM_Q;
  return a - t;
}

/*************************************************
* Name:        reduce32
* Description: For finite field element a, computes r in range [-q/2, q/2]
*              such that r = a (mod q)
* Arguments:   - int32_t a: input element
* Returns:     r
**************************************************/
int32_t reduce32(int32_t a) {
  int32_t t;
  t = (a + (1 << 22)) >> 23;
  t = a - t * DILITHIUM_Q;
  return t;
}

/*************************************************
* Name:        csubq
* Description: Conditional subtraction of q; given an integer a, returns
*              a - q if a >= q, and a otherwise.
* Arguments:   - int32_t a: input integer; must be in range [0, 2*q - 1]
* Returns:     a - q if a >= q, and a otherwise.
**************************************************/
int32_t csubq(int32_t a) {
  a -= DILITHIUM_Q;
  a += ((int32_t)a >> 31) & DILITHIUM_Q;
  return a;
}

/*************************************************
* Name:        freeze32
* Description: For finite field element a, computes standard representative
*              r in range [0, q-1] such that r = a (mod q)
* Arguments:   - int32_t a: input element
* Returns:     r
**************************************************/
int32_t freeze32(int32_t a) {
  int32_t t;
  t = reduce32(a);
  t += (t >> 31) & DILITHIUM_Q;
  return t;
}
