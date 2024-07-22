#include "params.h"
#include "poly.h"
#include "polyvec.h"
#include "reduce.h"
#include "fips202.h"
#include "sign.h"

/*
 * Shared ML-DSA-65 commitment + Fiat-Shamir challenge hash.
 *
 * Given the NTT-domain public matrix A (`mat`), the message digest `mu`, and a
 * sampled masking vector `y`, this computes the commitment w = A*y, its high/low
 * decomposition (w1, w0), and the challenge seed c_tilde = H(mu || w1).
 *
 * This is the single source of truth for the commitment path. Both the stock
 * signer (sign.c) and the fused PoW miner (pq-pow-dilithium/pow_fused.c) call it,
 * so the two implementations cannot drift apart.
 */
void dilithium_commit(polyveck *w1, polyveck *w0, uint8_t c_tilde[SEEDBYTES],
                      const polyvecl mat[DILITHIUM_K], const uint8_t mu[CRHBYTES],
                      const polyvecl *y) {
  unsigned int i, j;
  polyvecl yhat = *y;
  uint8_t w1_packed[DILITHIUM_K * 128];
  shake256state state;

  /* w = A * y, brought back to the normal domain. */
  polyvecl_ntt(&yhat);
  polyvec_matrix_pointwise_montgomery(w1, mat, &yhat);
  polyveck_invntt_tomont(w1);

  /* Normalize, then decompose w = w1 * 2*gamma2 + w0. */
  for (i = 0; i < DILITHIUM_K; i++) {
    for (j = 0; j < DILITHIUM_N; j++) {
      w1->vec[i].coeffs[j] = freeze32(w1->vec[i].coeffs[j]);
    }
  }
  for (i = 0; i < DILITHIUM_K; i++) {
    poly_decompose(&w1->vec[i], &w0->vec[i], &w1->vec[i]);
  }

  /* Pack w1 (4 bits per coefficient) and hash c_tilde = H(mu || w1). */
  for (i = 0; i < DILITHIUM_K; i++) {
    for (j = 0; j < DILITHIUM_N / 2; j++) {
      w1_packed[i * 128 + j] =
          (uint8_t)(w1->vec[i].coeffs[2 * j] | (w1->vec[i].coeffs[2 * j + 1] << 4));
    }
  }

  shake256_init(&state);
  shake256_absorb(&state, mu, CRHBYTES);
  shake256_absorb(&state, w1_packed, DILITHIUM_K * 128);
  shake256_finalize(&state);
  shake256_squeezerx(&state, c_tilde, SEEDBYTES);
}
