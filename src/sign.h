#ifndef SIGN_H
#define SIGN_H

#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "polyvec.h"

int pqc_dilithium_keypair(uint8_t *pk, uint8_t *sk);

int pqc_dilithium_sign(uint8_t *sig, size_t *siglen,
                       const uint8_t *msg, size_t msglen,
                       const uint8_t *sk);

int pqc_dilithium_verify(const uint8_t *sig, size_t siglen,
                         const uint8_t *msg, size_t msglen,
                         const uint8_t *pk);

void challenge(poly *c, const uint8_t c_tilde[SEEDBYTES]);

/*
 * Shared commitment path: w = A*y, decomposed into (w1, w0), plus the
 * Fiat-Shamir challenge seed c_tilde = H(mu || w1). `mat` must already be in the
 * NTT domain. Used by both the stock signer and the fused PoW miner so they
 * cannot diverge.
 */
void dilithium_commit(polyveck *w1, polyveck *w0, uint8_t c_tilde[SEEDBYTES],
                      const polyvecl mat[DILITHIUM_K], const uint8_t mu[CRHBYTES],
                      const polyvecl *y);

#endif
