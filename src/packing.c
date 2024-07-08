#include <stdint.h>
#include <string.h>
#include "params.h"
#include "poly.h"
#include "polyvec.h"
#include "packing.h"

void polyt1_pack(uint8_t *r, const poly *a) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N / 4; i++) {
    r[5 * i + 0] = (a->coeffs[4 * i + 0] >> 0) & 0xFF;
    r[5 * i + 1] = ((a->coeffs[4 * i + 0] >> 8) & 0x03) | ((a->coeffs[4 * i + 1] << 2) & 0xFC);
    r[5 * i + 2] = ((a->coeffs[4 * i + 1] >> 6) & 0x0F) | ((a->coeffs[4 * i + 2] << 4) & 0xF0);
    r[5 * i + 3] = ((a->coeffs[4 * i + 2] >> 4) & 0x3F) | ((a->coeffs[4 * i + 3] << 6) & 0xC0);
    r[5 * i + 4] = (a->coeffs[4 * i + 3] >> 2) & 0xFF;
  }
}

void polyt1_unpack(poly *r, const uint8_t *a) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N / 4; i++) {
    r->coeffs[4 * i + 0] = ((a[5 * i + 0] >> 0) | ((uint32_t)a[5 * i + 1] << 8)) & 0x3FF;
    r->coeffs[4 * i + 1] = ((a[5 * i + 1] >> 2) | ((uint32_t)a[5 * i + 2] << 6)) & 0x3FF;
    r->coeffs[4 * i + 2] = ((a[5 * i + 2] >> 4) | ((uint32_t)a[5 * i + 3] << 4)) & 0x3FF;
    r->coeffs[4 * i + 3] = ((a[5 * i + 3] >> 6) | ((uint32_t)a[5 * i + 4] << 2)) & 0x3FF;
  }
}

void polyt0_pack(uint8_t *r, const poly *a) {
  unsigned int i;
  uint32_t t[8];
  for (i = 0; i < DILITHIUM_N / 8; i++) {
    t[0] = (1 << 12) - a->coeffs[8 * i + 0];
    t[1] = (1 << 12) - a->coeffs[8 * i + 1];
    t[2] = (1 << 12) - a->coeffs[8 * i + 2];
    t[3] = (1 << 12) - a->coeffs[8 * i + 3];
    t[4] = (1 << 12) - a->coeffs[8 * i + 4];
    t[5] = (1 << 12) - a->coeffs[8 * i + 5];
    t[6] = (1 << 12) - a->coeffs[8 * i + 6];
    t[7] = (1 << 12) - a->coeffs[8 * i + 7];

    r[13 * i + 0]  = t[0] & 0xFF;
    r[13 * i + 1]  = (t[0] >> 8) | ((t[1] & 0x07) << 5);
    r[13 * i + 2]  = (t[1] >> 3) & 0xFF;
    r[13 * i + 3]  = (t[1] >> 11) | ((t[2] & 0x3F) << 2);
    r[13 * i + 4]  = (t[2] >> 6) | ((t[3] & 0x01) << 7);
    r[13 * i + 5]  = (t[3] >> 1) & 0xFF;
    r[13 * i + 6]  = (t[3] >> 9) | ((t[4] & 0x0F) << 4);
    r[13 * i + 7]  = (t[4] >> 4) & 0xFF;
    r[13 * i + 8]  = (t[4] >> 12) | ((t[5] & 0x7F) << 1);
    r[13 * i + 9]  = (t[5] >> 7) | ((t[6] & 0x03) << 6);
    r[13 * i + 10] = (t[6] >> 2) & 0xFF;
    r[13 * i + 11] = (t[6] >> 10) | ((t[7] & 0x1F) << 3);
    r[13 * i + 12] = (t[7] >> 5) & 0xFF;
  }
}

void polyt0_unpack(poly *r, const uint8_t *a) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N / 8; i++) {
    uint32_t t[8];
    t[0] = a[13 * i + 0] | ((uint32_t)(a[13 * i + 1] & 0x1F) << 8);
    t[1] = (a[13 * i + 1] >> 5) | ((uint32_t)a[13 * i + 2] << 3) | ((uint32_t)(a[13 * i + 3] & 0x03) << 11);
    t[2] = (a[13 * i + 3] >> 2) | ((uint32_t)(a[13 * i + 4] & 0x7F) << 6);
    t[3] = (a[13 * i + 4] >> 7) | ((uint32_t)a[13 * i + 5] << 1) | ((uint32_t)(a[13 * i + 6] & 0x0F) << 9);
    t[4] = (a[13 * i + 6] >> 4) | ((uint32_t)a[13 * i + 7] << 4) | ((uint32_t)(a[13 * i + 8] & 0x01) << 12);
    t[5] = (a[13 * i + 8] >> 1) | ((uint32_t)(a[13 * i + 9] & 0x3F) << 7);
    t[6] = (a[13 * i + 9] >> 6) | ((uint32_t)a[13 * i + 10] << 2) | ((uint32_t)(a[13 * i + 11] & 0x07) << 10);
    t[7] = (a[13 * i + 11] >> 3) | ((uint32_t)a[13 * i + 12] << 5);

    r->coeffs[8 * i + 0] = (1 << 12) - t[0];
    r->coeffs[8 * i + 1] = (1 << 12) - t[1];
    r->coeffs[8 * i + 2] = (1 << 12) - t[2];
    r->coeffs[8 * i + 3] = (1 << 12) - t[3];
    r->coeffs[8 * i + 4] = (1 << 12) - t[4];
    r->coeffs[8 * i + 5] = (1 << 12) - t[5];
    r->coeffs[8 * i + 6] = (1 << 12) - t[6];
    r->coeffs[8 * i + 7] = (1 << 12) - t[7];
  }
}

void polyeta_pack(uint8_t *r, const poly *a) {
  unsigned int i;
  uint8_t t[2];
  for (i = 0; i < DILITHIUM_N / 2; i++) {
    t[0] = DILITHIUM_ETA - a->coeffs[2 * i + 0];
    t[1] = DILITHIUM_ETA - a->coeffs[2 * i + 1];
    r[i] = t[0] | (t[1] << 4);
  }
}

void polyeta_unpack(poly *r, const uint8_t *a) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N / 2; i++) {
    r->coeffs[2 * i + 0] = DILITHIUM_ETA - (a[i] & 0x0F);
    r->coeffs[2 * i + 1] = DILITHIUM_ETA - (a[i] >> 4);
  }
}

void polyz_pack(uint8_t *r, const poly *a) {
  unsigned int i;
  uint32_t t[2];
  for (i = 0; i < DILITHIUM_N / 2; i++) {
    t[0] = DILITHIUM_GAMMA1 - a->coeffs[2 * i + 0];
    t[1] = DILITHIUM_GAMMA1 - a->coeffs[2 * i + 1];

    r[5 * i + 0] = t[0] & 0xFF;
    r[5 * i + 1] = (t[0] >> 8) & 0xFF;
    r[5 * i + 2] = ((t[0] >> 16) & 0x0F) | ((t[1] & 0x0F) << 4);
    r[5 * i + 3] = (t[1] >> 4) & 0xFF;
    r[5 * i + 4] = (t[1] >> 12) & 0xFF;
  }
}

void polyz_unpack(poly *r, const uint8_t *a) {
  unsigned int i;
  for (i = 0; i < DILITHIUM_N / 2; i++) {
    uint32_t t0 = a[5 * i + 0] | ((uint32_t)a[5 * i + 1] << 8) | ((uint32_t)(a[5 * i + 2] & 0x0F) << 16);
    uint32_t t1 = (a[5 * i + 2] >> 4) | ((uint32_t)a[5 * i + 3] << 4) | ((uint32_t)a[5 * i + 4] << 12);
    r->coeffs[2 * i + 0] = DILITHIUM_GAMMA1 - t0;
    r->coeffs[2 * i + 1] = DILITHIUM_GAMMA1 - t1;
  }
}

void pack_pk(uint8_t pk[PUBLICKEYBYTES], const uint8_t rho[SEEDBYTES], const polyveck *t1) {
  unsigned int i;
  memcpy(pk, rho, SEEDBYTES);
  for (i = 0; i < DILITHIUM_K; i++) {
    polyt1_pack(pk + SEEDBYTES + i * POLYT1_PACKEDBYTES, &t1->vec[i]);
  }
}

void unpack_pk(uint8_t rho[SEEDBYTES], polyveck *t1, const uint8_t pk[PUBLICKEYBYTES]) {
  unsigned int i;
  memcpy(rho, pk, SEEDBYTES);
  for (i = 0; i < DILITHIUM_K; i++) {
    polyt1_unpack(&t1->vec[i], pk + SEEDBYTES + i * POLYT1_PACKEDBYTES);
  }
}

void pack_sk(uint8_t sk[PRIVATEKEYBYTES],
             const uint8_t rho[SEEDBYTES],
             const uint8_t key[SEEDBYTES],
             const uint8_t tr[CRHBYTES],
             const polyvecl *s1,
             const polyveck *s2,
             const polyveck *t0) {
  unsigned int i;
  memcpy(sk, rho, SEEDBYTES);
  sk += SEEDBYTES;
  memcpy(sk, key, SEEDBYTES);
  sk += SEEDBYTES;
  memcpy(sk, tr, CRHBYTES);
  sk += CRHBYTES;

  for (i = 0; i < DILITHIUM_L; i++) {
    polyeta_pack(sk + i * 128, &s1->vec[i]);
  }
  sk += DILITHIUM_L * 128;

  for (i = 0; i < DILITHIUM_K; i++) {
    polyeta_pack(sk + i * 128, &s2->vec[i]);
  }
  sk += DILITHIUM_K * 128;

  for (i = 0; i < DILITHIUM_K; i++) {
    polyt0_pack(sk + i * POLYT0_PACKEDBYTES, &t0->vec[i]);
  }
}

void unpack_sk(uint8_t rho[SEEDBYTES],
               uint8_t key[SEEDBYTES],
               uint8_t tr[CRHBYTES],
               polyvecl *s1,
               polyveck *s2,
               polyveck *t0,
               const uint8_t sk[PRIVATEKEYBYTES]) {
  unsigned int i;
  memcpy(rho, sk, SEEDBYTES);
  sk += SEEDBYTES;
  memcpy(key, sk, SEEDBYTES);
  sk += SEEDBYTES;
  memcpy(tr, sk, CRHBYTES);
  sk += CRHBYTES;

  for (i = 0; i < DILITHIUM_L; i++) {
    polyeta_unpack(&s1->vec[i], sk + i * 128);
  }
  sk += DILITHIUM_L * 128;

  for (i = 0; i < DILITHIUM_K; i++) {
    polyeta_unpack(&s2->vec[i], sk + i * 128);
  }
  sk += DILITHIUM_K * 128;

  for (i = 0; i < DILITHIUM_K; i++) {
    polyt0_unpack(&t0->vec[i], sk + i * POLYT0_PACKEDBYTES);
  }
}

void pack_sig(uint8_t sig[SIGNATUREBYTES], const uint8_t c_tilde[SEEDBYTES], const polyvecl *z, const polyveck *h) {
  unsigned int i, j, k;
  memcpy(sig, c_tilde, SEEDBYTES);
  sig += SEEDBYTES;

  for (i = 0; i < DILITHIUM_L; i++) {
    polyz_pack(sig + i * 640, &z->vec[i]);
  }
  sig += DILITHIUM_L * 640;

  /* Pack hint vector h:
   * Stores the indices of the non-zero coefficients of h.
   * Then writes counts of non-zero coefficients in each polynomial. */
  for (i = 0; i < DILITHIUM_OMEGA + DILITHIUM_K; i++) {
    sig[i] = 0;
  }

  k = 0;
  for (i = 0; i < DILITHIUM_K; i++) {
    for (j = 0; j < DILITHIUM_N; j++) {
      if (h->vec[i].coeffs[j] != 0) {
        sig[k++] = j;
      }
    }
    sig[DILITHIUM_OMEGA + i] = k;
  }
}

int unpack_sig(uint8_t c_tilde[SEEDBYTES], polyvecl *z, polyveck *h, const uint8_t sig[SIGNATUREBYTES]) {
  unsigned int i, j, k, limit;
  memcpy(c_tilde, sig, SEEDBYTES);
  sig += SEEDBYTES;

  for (i = 0; i < DILITHIUM_L; i++) {
    polyz_unpack(&z->vec[i], sig + i * 640);
  }
  sig += DILITHIUM_L * 640;

  /* Unpack hint vector h */
  for (i = 0; i < DILITHIUM_K; i++) {
    for (j = 0; j < DILITHIUM_N; j++) {
      h->vec[i].coeffs[j] = 0;
    }
  }

  k = 0;
  for (i = 0; i < DILITHIUM_K; i++) {
    limit = sig[DILITHIUM_OMEGA + i];
    if (limit < k || limit > DILITHIUM_OMEGA) {
      return -1;
    }
    for (j = k; j < limit; j++) {
      h->vec[i].coeffs[sig[j]] = 1;
    }
    k = limit;
  }

  /* Any remaining positions must be 0 */
  for (j = k; j < DILITHIUM_OMEGA; j++) {
    if (sig[j] != 0) {
      return -1;
    }
  }

  return 0;
}
