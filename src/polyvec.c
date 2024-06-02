#include <stdint.h>
#include "params.h"
#include "poly.h"
#include "polyvec.h"

void polyvecl_uniform_eta(polyvecl *v, const uint8_t seed[CRHBYTES], uint16_t nonce) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_L; i++) {
    poly_uniform_eta(&v->vec[i], seed, nonce + i);
  }
}

void polyvecl_uniform_gamma1(polyvecl *v, const uint8_t seed[CRHBYTES], uint16_t nonce) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_L; i++) {
    poly_uniform_gamma1(&v->vec[i], seed, nonce * DILITHIUM_L + i);
  }
}

void polyvecl_reduce(polyvecl *v) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_L; i++) {
    poly_reduce(&v->vec[i]);
  }
}

void polyvecl_freeze(polyvecl *v) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_L; i++) {
    poly_freeze(&v->vec[i]);
  }
}

void polyvecl_add(polyvecl *c, const polyvecl *a, const polyvecl *b) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_L; i++) {
    poly_add(&c->vec[i], &a->vec[i], &b->vec[i]);
  }
}

void polyvecl_ntt(polyvecl *v) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_L; i++) {
    poly_ntt(&v->vec[i]);
  }
}

void polyvecl_invntt_tomont(polyvecl *v) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_L; i++) {
    poly_invntt_tomont(&v->vec[i]);
  }
}

void polyvecl_pointwise_acc_montgomery(poly *w, const polyvecl *u, const polyvecl *v) {
  unsigned int i;
  poly t;

  poly_pointwise_montgomery(w, &u->vec[0], &v->vec[0]);
  for (i = 1; i < DILITHIUM_L; i++) {
    poly_pointwise_montgomery(&t, &u->vec[i], &v->vec[i]);
    poly_add(w, w, &t);
  }
}

int polyvecl_chknorm(const polyvecl *v, int32_t B) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_L; i++) {
    if (poly_chknorm(&v->vec[i], B)) {
      return 1;
    }
  }
  return 0;
}

void polyveck_uniform_eta(polyveck *v, const uint8_t seed[CRHBYTES], uint16_t nonce) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_K; i++) {
    poly_uniform_eta(&v->vec[i], seed, nonce + i);
  }
}

void polyveck_reduce(polyveck *v) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_K; i++) {
    poly_reduce(&v->vec[i]);
  }
}

void polyveck_csubq(polyveck *v) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_K; i++) {
    poly_csubq(&v->vec[i]);
  }
}

void polyveck_freeze(polyveck *v) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_K; i++) {
    poly_freeze(&v->vec[i]);
  }
}

void polyveck_add(polyveck *c, const polyveck *a, const polyveck *b) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_K; i++) {
    poly_add(&c->vec[i], &a->vec[i], &b->vec[i]);
  }
}

void polyveck_sub(polyveck *c, const polyveck *a, const polyveck *b) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_K; i++) {
    poly_sub(&c->vec[i], &a->vec[i], &b->vec[i]);
  }
}

void polyveck_shiftl(polyveck *v) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_K; i++) {
    poly_shiftl(&v->vec[i]);
  }
}

void polyveck_ntt(polyveck *v) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_K; i++) {
    poly_ntt(&v->vec[i]);
  }
}

void polyveck_invntt_tomont(polyveck *v) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_K; i++) {
    poly_invntt_tomont(&v->vec[i]);
  }
}

int polyveck_chknorm(const polyveck *v, int32_t B) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_K; i++) {
    if (poly_chknorm(&v->vec[i], B)) {
      return 1;
    }
  }
  return 0;
}

void polyvec_matrix_pointwise_montgomery(polyveck *t, const polyvecl mat[DILITHIUM_K], const polyvecl *v) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_K; i++) {
    polyvecl_pointwise_acc_montgomery(&t->vec[i], &mat[i], v);
  }
}

void polyvec_matrix_expand(polyvecl mat[DILITHIUM_K], const uint8_t rho[SEEDBYTES]) {
  unsigned int i, j;
  for (i = 0; i < DILITHIUM_K; i++) {
    for (j = 0; j < DILITHIUM_L; j++) {
      poly_uniform(&mat[i].vec[j], rho, (uint16_t)((i << 8) + j));
    }
  }
}
