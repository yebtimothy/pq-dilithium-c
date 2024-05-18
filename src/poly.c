#include <stdint.h>
#include <string.h>
#include "params.h"
#include "reduce.h"
#include "ntt.h"
#include "fips202.h"
#include "poly.h"

void poly_reduce(poly *a) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N; i++) {
    a->coeffs[i] = barrett_reduce(a->coeffs[i]);
  }
}

void poly_csubq(poly *a) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N; i++) {
    a->coeffs[i] = csubq(a->coeffs[i]);
  }
}

void poly_freeze(poly *a) {
  poly_reduce(a);
  poly_csubq(a);
}

void poly_add(poly *c, const poly *a, const poly *b) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N; i++) {
    c->coeffs[i] = a->coeffs[i] + b->coeffs[i];
  }
}

void poly_sub(poly *c, const poly *a, const poly *b) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N; i++) {
    c->coeffs[i] = a->coeffs[i] - b->coeffs[i];
  }
}

void poly_shiftl(poly *a) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N; i++) {
    a->coeffs[i] <<= DILITHIUM_D;
  }
}

void poly_ntt(poly *a) {
  ntt(a->coeffs);
}

void poly_invntt_tomont(poly *a) {
  invntt_tomont(a->coeffs);
}

void poly_pointwise_montgomery(poly *c, const poly *a, const poly *b) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N; i++) {
    c->coeffs[i] = montgomery_reduce((int64_t)a->coeffs[i] * b->coeffs[i]);
  }
}

int poly_chknorm(const poly *a, int32_t B) {
  unsigned int i;
  int32_t t;

  if (B < 0) {
    return 1;
  }

  for (i = 0; i < DILITHIUM_N; i++) {
    t = a->coeffs[i] >> 31;
    t = a->coeffs[i] - (t & 2 * a->coeffs[i]); // Absolute value
    if (t >= B) {
      return 1;
    }
  }
  return 0;
}

void poly_uniform(poly *a, const uint8_t seed[SEEDBYTES], uint16_t nonce) {
  unsigned int ctr = 0;
  uint8_t buf[SHAKE128_RATE * 3];
  shake128state state;
  uint8_t extseed[SEEDBYTES + 2];
  uint32_t val;
  unsigned int pos = 0;

  memcpy(extseed, seed, SEEDBYTES);
  extseed[SEEDBYTES] = nonce & 0xFF;
  extseed[SEEDBYTES + 1] = (nonce >> 8) & 0xFF;

  shake128_init(&state);
  shake128_absorb(&state, extseed, SEEDBYTES + 2);
  shake128_finalize(&state);

  shake128_squeezerx(&state, buf, SHAKE128_RATE * 3);

  while (ctr < DILITHIUM_N) {
    if (pos + 3 > SHAKE128_RATE * 3) {
      shake128_squeezerx(&state, buf, SHAKE128_RATE * 3);
      pos = 0;
    }
    val = (uint32_t)buf[pos] | ((uint32_t)buf[pos+1] << 8) | ((uint32_t)buf[pos+2] << 16);
    val &= 0x7FFFFF;
    pos += 3;

    if (val < DILITHIUM_Q) {
      a->coeffs[ctr++] = val;
    }
  }
}

void poly_uniform_eta(poly *a, const uint8_t seed[CRHBYTES], uint16_t nonce) {
  unsigned int ctr = 0;
  uint8_t buf[SHAKE256_RATE];
  shake256state state;
  uint8_t extseed[CRHBYTES + 2];
  uint8_t val0, val1;
  unsigned int pos = 0;

  memcpy(extseed, seed, CRHBYTES);
  extseed[CRHBYTES] = nonce & 0xFF;
  extseed[CRHBYTES + 1] = (nonce >> 8) & 0xFF;

  shake256_init(&state);
  shake256_absorb(&state, extseed, CRHBYTES + 2);
  shake256_finalize(&state);

  shake256_squeezerx(&state, buf, SHAKE256_RATE);

  while (ctr < DILITHIUM_N) {
    if (pos >= SHAKE256_RATE) {
      shake256_squeezerx(&state, buf, SHAKE256_RATE);
      pos = 0;
    }
    val0 = buf[pos] & 0x0F;
    val1 = buf[pos] >> 4;
    pos++;

    if (val0 < 9) {
      a->coeffs[ctr++] = DILITHIUM_ETA - val0;
    }
    if (ctr < DILITHIUM_N && val1 < 9) {
      a->coeffs[ctr++] = DILITHIUM_ETA - val1;
    }
  }
}

void poly_uniform_gamma1(poly *a, const uint8_t seed[CRHBYTES], uint16_t nonce) {
  unsigned int ctr = 0;
  uint8_t buf[SHAKE256_RATE * 5];
  shake256state state;
  uint8_t extseed[CRHBYTES + 2];
  uint32_t val0, val1;
  unsigned int pos = 0;

  memcpy(extseed, seed, CRHBYTES);
  extseed[CRHBYTES] = nonce & 0xFF;
  extseed[CRHBYTES + 1] = (nonce >> 8) & 0xFF;

  shake256_init(&state);
  shake256_absorb(&state, extseed, CRHBYTES + 2);
  shake256_finalize(&state);

  shake256_squeezerx(&state, buf, SHAKE256_RATE * 5);

  while (ctr < DILITHIUM_N) {
    if (pos + 5 > SHAKE256_RATE * 5) {
      shake256_squeezerx(&state, buf, SHAKE256_RATE * 5);
      pos = 0;
    }
    val0 = ((uint32_t)buf[pos] | ((uint32_t)buf[pos+1] << 8) | ((uint32_t)buf[pos+2] << 16)) & 0xFFFFF;
    val1 = (((uint32_t)buf[pos+2] >> 4) | ((uint32_t)buf[pos+3] << 4) | ((uint32_t)buf[pos+4] << 12)) & 0xFFFFF;
    pos += 5;

    a->coeffs[ctr++] = (int32_t)val0 - DILITHIUM_GAMMA1;
    if (ctr < DILITHIUM_N) {
      a->coeffs[ctr++] = (int32_t)val1 - DILITHIUM_GAMMA1;
    }
  }
}

static int32_t power2round_element(int32_t *a0, int32_t a) {
  int32_t a1;
  a1 = (a + (1 << (DILITHIUM_D - 1)) - 1) >> DILITHIUM_D;
  *a0 = a - (a1 << DILITHIUM_D);
  return a1;
}

static int32_t decompose_element(int32_t *a0, int32_t a) {
  int32_t a1;
  a1 = (a + 127) >> 7;
  a1 = (a1 * 1025 + (1 << 21)) >> 22;
  a1 &= 15;
  *a0 = a - a1 * 2 * DILITHIUM_GAMMA2;
  *a0 -= (((DILITHIUM_Q - 1) / 2 - *a0) >> 31) & DILITHIUM_Q;
  return a1;
}

static unsigned int make_hint_element(int32_t a0, int32_t a1) {
  if (a0 > DILITHIUM_GAMMA2 || a0 < -DILITHIUM_GAMMA2 ||
      (a0 == -DILITHIUM_GAMMA2 && a1 != 0)) {
    return 1;
  }
  return 0;
}

static int32_t use_hint_element(int32_t a, unsigned int hint) {
  int32_t a0, a1;
  a1 = decompose_element(&a0, a);
  if (hint == 0) {
    return a1;
  }
  if (a0 > 0) {
    return (a1 + 1) & 15;
  } else {
    return (a1 - 1) & 15;
  }
}

void poly_power2round(poly *a1, poly *a0, const poly *a) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N; i++) {
    a1->coeffs[i] = power2round_element(&a0->coeffs[i], a->coeffs[i]);
  }
}

void poly_decompose(poly *a1, poly *a0, const poly *a) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N; i++) {
    a1->coeffs[i] = decompose_element(&a0->coeffs[i], a->coeffs[i]);
  }
}

unsigned int poly_make_hint(poly *h, const poly *a0, const poly *a1) {
  unsigned int i, s = 0;
  for (i = 0; i < DILITHIUM_N; i++) {
    h->coeffs[i] = make_hint_element(a0->coeffs[i], a1->coeffs[i]);
    s += h->coeffs[i];
  }
  return s;
}

void poly_use_hint(poly *b, const poly *a, const poly *h) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N; i++) {
    b->coeffs[i] = use_hint_element(a->coeffs[i], h->coeffs[i]);
  }
}
