#ifndef POLYVEC_H
#define POLYVEC_H

#include <stdint.h>
#include "params.h"
#include "poly.h"

typedef struct {
  poly vec[DILITHIUM_L];
} polyvecl;

typedef struct {
  poly vec[DILITHIUM_K];
} polyveck;

void polyvecl_uniform_eta(polyvecl *v, const uint8_t seed[CRHBYTES], uint16_t nonce);
void polyvecl_uniform_gamma1(polyvecl *v, const uint8_t seed[CRHBYTES], uint16_t nonce);
void polyvecl_reduce(polyvecl *v);
void polyvecl_freeze(polyvecl *v);
void polyvecl_add(polyvecl *c, const polyvecl *a, const polyvecl *b);
void polyvecl_ntt(polyvecl *v);
void polyvecl_invntt_tomont(polyvecl *v);
void polyvecl_pointwise_acc_montgomery(poly *w, const polyvecl *u, const polyvecl *v);
int polyvecl_chknorm(const polyvecl *v, int32_t B);

void polyveck_uniform_eta(polyveck *v, const uint8_t seed[CRHBYTES], uint16_t nonce);
void polyveck_reduce(polyveck *v);
void polyveck_csubq(polyveck *v);
void polyveck_freeze(polyveck *v);
void polyveck_add(polyveck *c, const polyveck *a, const polyveck *b);
void polyveck_sub(polyveck *c, const polyveck *a, const polyveck *b);
void polyveck_shiftl(polyveck *v);
void polyveck_ntt(polyveck *v);
void polyveck_invntt_tomont(polyveck *v);
int polyveck_chknorm(const polyveck *v, int32_t B);

void polyvec_matrix_pointwise_montgomery(polyveck *t, const polyvecl mat[DILITHIUM_K], const polyvecl *v);

/* Expand the public seed rho into the matrix A (normal domain). Shared by the
 * keygen/sign paths and the fused PoW miner so the expansion exists once. */
void polyvec_matrix_expand(polyvecl mat[DILITHIUM_K], const uint8_t rho[SEEDBYTES]);

#endif
