#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "params.h"
#include "fips202.h"
#include "randombytes.h"
#include "reduce.h"
#include "poly.h"
#include "polyvec.h"
#include "packing.h"
#include "sign.h"

void challenge(poly *c, const uint8_t c_tilde[SEEDBYTES]) {
  unsigned int i, pos;
  uint64_t signs;
  uint8_t buf[SHAKE256_RATE];
  shake256state state;

  shake256_init(&state);
  shake256_absorb(&state, c_tilde, SEEDBYTES);
  shake256_finalize(&state);
  shake256_squeezerx(&state, buf, SHAKE256_RATE);

  for (i = 0; i < DILITHIUM_N; i++) {
    c->coeffs[i] = 0;
  }

  signs = 0;
  for (i = 0; i < 8; i++) {
    signs |= (uint64_t)buf[i] << (8 * i);
  }
  pos = 8;

  for (i = DILITHIUM_N - DILITHIUM_TAU; i < DILITHIUM_N; i++) {
    unsigned int j;
    do {
      if (pos >= SHAKE256_RATE) {
        shake256_squeezerx(&state, buf, SHAKE256_RATE);
        pos = 0;
      }
      j = buf[pos++];
    } while (j > i);

    c->coeffs[i] = c->coeffs[j];
    c->coeffs[j] = 1 - 2 * (signs & 1);
    signs >>= 1;
  }
}

int pqc_dilithium_keypair(uint8_t *pk, uint8_t *sk) {
  uint8_t seedbuf[3 * SEEDBYTES];
  uint8_t tr[CRHBYTES];
  const uint8_t *rho, *rhoprime, *key;
  polyvecl mat[DILITHIUM_K];
  polyvecl s1;
  polyveck s2, t, t1, t0;

  /* 1. Generate random seed */
  randombytes(seedbuf, SEEDBYTES);
  shake256(seedbuf, 3 * SEEDBYTES, seedbuf, SEEDBYTES);
  rho = seedbuf;
  rhoprime = seedbuf + SEEDBYTES;
  key = seedbuf + 2 * SEEDBYTES;

  /* 2. Expand matrix A and convert to NTT domain */
  polyvec_matrix_expand(mat, rho);
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    polyvecl_ntt(&mat[i]);
  }

  /* 3. Sample secret vectors s1, s2 */
  polyvecl_uniform_eta(&s1, rhoprime, 0);
  polyveck_uniform_eta(&s2, rhoprime, DILITHIUM_L);

  /* 4. Compute t = A * s1 + s2 */
  polyvecl s1_ntt = s1;
  polyvecl_ntt(&s1_ntt);
  polyvec_matrix_pointwise_montgomery(&t, mat, &s1_ntt);
  polyveck_invntt_tomont(&t);
  polyveck_add(&t, &t, &s2);

  /* 5. Decompose t = t1 * 2^d + t0 */
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    for (unsigned int j = 0; j < DILITHIUM_N; j++) {
      t.vec[i].coeffs[j] = freeze32(t.vec[i].coeffs[j]);
    }
  }
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    poly_power2round(&t1.vec[i], &t0.vec[i], &t.vec[i]);
  }

  /* 6. Pack Public Key */
  pack_pk(pk, rho, &t1);

  /* 7. Compute tr = CRH(rho || t1) */
  shake256(tr, CRHBYTES, pk, PUBLICKEYBYTES);

  /* 8. Pack Secret Key */
  pack_sk(sk, rho, key, tr, &s1, &s2, &t0);

  return 0;
}

int pqc_dilithium_sign(uint8_t *sig, size_t *siglen,
                       const uint8_t *msg, size_t msglen,
                       const uint8_t *sk) {
  uint8_t rho[SEEDBYTES], key[SEEDBYTES], tr[CRHBYTES];
  uint8_t mu[CRHBYTES], rhoprime[CRHBYTES];
  uint16_t nonce = 0;
  polyvecl mat[DILITHIUM_K];
  polyvecl s1, y, z;
  polyveck s2, t0, w1, w0, h;
  poly cp;
  shake256state state;
  uint8_t c_tilde[SEEDBYTES];

  /* 1. Unpack secret key */
  unpack_sk(rho, key, tr, &s1, &s2, &t0, sk);

  /* 2. Compute mu = CRH(tr || msg) */
  shake256_init(&state);
  shake256_absorb(&state, tr, CRHBYTES);
  shake256_absorb(&state, msg, msglen);
  shake256_finalize(&state);
  shake256_squeezerx(&state, mu, CRHBYTES);

  /* 3. Compute rhoprime = CRH(key || mu) */
  shake256_init(&state);
  shake256_absorb(&state, key, SEEDBYTES);
  shake256_absorb(&state, mu, CRHBYTES);
  shake256_finalize(&state);
  shake256_squeezerx(&state, rhoprime, CRHBYTES);

  /* 4. Expand matrix A and convert to NTT domain */
  polyvec_matrix_expand(mat, rho);
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    polyvecl_ntt(&mat[i]);
  }

  polyvecl_ntt(&s1);
  polyveck_ntt(&s2);
  polyveck_ntt(&t0);

  rej:
  if (nonce > 10000) {
    return -99;
  }
  /* 5. Sample y */
  polyvecl_uniform_gamma1(&y, rhoprime, nonce++);

  /* 6-8. Commitment w = A*y, decomposition (w1, w0), and Fiat-Shamir hash
   * c_tilde = H(mu || w1). Shared with the fused PoW miner via dilithium_commit()
   * so the two commitment paths cannot drift apart. */
  dilithium_commit(&w1, &w0, c_tilde, mat, mu, &y);

  /* 9. Sample challenge c */
  challenge(&cp, c_tilde);
  poly_ntt(&cp);

  /* 10. Compute z = y + c * s1 */
  for (unsigned int i = 0; i < DILITHIUM_L; i++) {
    poly_pointwise_montgomery(&z.vec[i], &cp, &s1.vec[i]);
    poly_invntt_tomont(&z.vec[i]);
  }
  polyvecl_add(&z, &y, &z);
  for (unsigned int i = 0; i < DILITHIUM_L; i++) {
    for (unsigned int j = 0; j < DILITHIUM_N; j++) {
      z.vec[i].coeffs[j] = reduce32(z.vec[i].coeffs[j]);
    }
  }

  /* 11. Check bounds on z */
  if (polyvecl_chknorm(&z, DILITHIUM_GAMMA1 - DILITHIUM_BETA)) {
    goto rej;
  }

  /* 12. Compute w0 - c * s2 */
  polyveck cs2;
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    poly_pointwise_montgomery(&cs2.vec[i], &cp, &s2.vec[i]);
    poly_invntt_tomont(&cs2.vec[i]);
  }
  polyveck_sub(&w0, &w0, &cs2);
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    for (unsigned int j = 0; j < DILITHIUM_N; j++) {
      w0.vec[i].coeffs[j] = reduce32(w0.vec[i].coeffs[j]);
    }
  }

  /* 13. Check bounds on w0 - c * s2 */
  if (polyveck_chknorm(&w0, DILITHIUM_GAMMA2 - DILITHIUM_BETA)) {
    goto rej;
  }

  /* 14. Compute c * t0 */
  polyveck ct0;
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    poly_pointwise_montgomery(&ct0.vec[i], &cp, &t0.vec[i]);
    poly_invntt_tomont(&ct0.vec[i]);
  }
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    for (unsigned int j = 0; j < DILITHIUM_N; j++) {
      ct0.vec[i].coeffs[j] = reduce32(ct0.vec[i].coeffs[j]);
    }
  }
  if (polyveck_chknorm(&ct0, DILITHIUM_GAMMA2)) {
    // optional check
  }

  /* 15. Make Hint */
  polyveck_add(&w0, &w0, &ct0);
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    for (unsigned int j = 0; j < DILITHIUM_N; j++) {
      w0.vec[i].coeffs[j] = reduce32(w0.vec[i].coeffs[j]);
    }
  }
  unsigned int hints_count = 0;
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    hints_count += poly_make_hint(&h.vec[i], &w0.vec[i], &w1.vec[i]);
  }

  if (hints_count > DILITHIUM_OMEGA) {
    goto rej;
  }

  /* 16. Pack signature */
  pack_sig(sig, c_tilde, &z, &h);
  *siglen = SIGNATUREBYTES;

  return 0;
}

int pqc_dilithium_verify(const uint8_t *sig, size_t siglen,
                         const uint8_t *msg, size_t msglen,
                         const uint8_t *pk) {
  uint8_t rho[SEEDBYTES];
  uint8_t mu[CRHBYTES];
  uint8_t c_tilde[SEEDBYTES];
  polyveck t1, w1, h;
  polyvecl z;
  poly cp;
  polyvecl mat[DILITHIUM_K];
  shake256state state;

  if (siglen != SIGNATUREBYTES) {
    return -1;
  }

  /* 1. Unpack public key and signature */
  unpack_pk(rho, &t1, pk);
  if (unpack_sig(c_tilde, &z, &h, sig)) {
    return -1;
  }

  /* 2. Check bounds on z */
  if (polyvecl_chknorm(&z, DILITHIUM_GAMMA1 - DILITHIUM_BETA)) {
    return -2;
  }

  /* 3. Compute mu = CRH(tr || msg) */
  uint8_t tr[CRHBYTES];
  shake256(tr, CRHBYTES, pk, PUBLICKEYBYTES);

  shake256_init(&state);
  shake256_absorb(&state, tr, CRHBYTES);
  shake256_absorb(&state, msg, msglen);
  shake256_finalize(&state);
  shake256_squeezerx(&state, mu, CRHBYTES);

  /* 4. Sample challenge c */
  challenge(&cp, c_tilde);

  /* 5. Expand matrix A and convert to NTT domain */
  polyvec_matrix_expand(mat, rho);
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    polyvecl_ntt(&mat[i]);
  }

  /* 6. Compute w1 = A * z - c * t1 * 2^d */
  polyvecl_ntt(&z);
  polyvec_matrix_pointwise_montgomery(&w1, mat, &z);

  poly_ntt(&cp);
  polyveck_shiftl(&t1);
  polyveck_ntt(&t1);
  
  polyveck ct1;
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    poly_pointwise_montgomery(&ct1.vec[i], &cp, &t1.vec[i]);
  }
  polyveck_sub(&w1, &w1, &ct1);
  polyveck_invntt_tomont(&w1);
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    for (unsigned int j = 0; j < DILITHIUM_N; j++) {
      w1.vec[i].coeffs[j] = freeze32(w1.vec[i].coeffs[j]);
    }
  }

  /* 7. Reconstruct w1 using hint */
  polyveck w1_reconstructed;
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    poly_use_hint(&w1_reconstructed.vec[i], &w1.vec[i], &h.vec[i]);
  }

  /* 8. Verify c_tilde matches H(mu || reconstructed w1) */
  uint8_t w1_packed[DILITHIUM_K * 128];
  for (unsigned int i = 0; i < DILITHIUM_K; i++) {
    for (unsigned int j = 0; j < DILITHIUM_N / 2; j++) {
      w1_packed[i * 128 + j] = w1_reconstructed.vec[i].coeffs[2 * j] | (w1_reconstructed.vec[i].coeffs[2 * j + 1] << 4);
    }
  }

  uint8_t c_tilde_reconstructed[SEEDBYTES];
  shake256_init(&state);
  shake256_absorb(&state, mu, CRHBYTES);
  shake256_absorb(&state, w1_packed, DILITHIUM_K * 128);
  shake256_finalize(&state);
  shake256_squeezerx(&state, c_tilde_reconstructed, SEEDBYTES);

  if (memcmp(c_tilde, c_tilde_reconstructed, SEEDBYTES) != 0) {
    return -3;
  }

  return 0;
}
