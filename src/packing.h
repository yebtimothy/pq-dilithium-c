#ifndef PACKING_H
#define PACKING_H

#include <stdint.h>
#include "params.h"
#include "polyvec.h"

void pack_pk(uint8_t pk[PUBLICKEYBYTES], const uint8_t rho[SEEDBYTES], const polyveck *t1);
void unpack_pk(uint8_t rho[SEEDBYTES], polyveck *t1, const uint8_t pk[PUBLICKEYBYTES]);

void pack_sk(uint8_t sk[PRIVATEKEYBYTES],
             const uint8_t rho[SEEDBYTES],
             const uint8_t key[SEEDBYTES],
             const uint8_t tr[CRHBYTES],
             const polyvecl *s1,
             const polyveck *s2,
             const polyveck *t0);
             
void unpack_sk(uint8_t rho[SEEDBYTES],
               uint8_t key[SEEDBYTES],
               uint8_t tr[CRHBYTES],
               polyvecl *s1,
               polyveck *s2,
               polyveck *t0,
               const uint8_t sk[PRIVATEKEYBYTES]);

void pack_sig(uint8_t sig[SIGNATUREBYTES], const uint8_t c_tilde[SEEDBYTES], const polyvecl *z, const polyveck *h);
int unpack_sig(uint8_t c_tilde[SEEDBYTES], polyvecl *z, polyveck *h, const uint8_t sig[SIGNATUREBYTES]);

void polyt1_pack(uint8_t *r, const poly *a);
void polyt1_unpack(poly *r, const uint8_t *a);

void polyt0_pack(uint8_t *r, const poly *a);
void polyt0_unpack(poly *r, const uint8_t *a);

void polyeta_pack(uint8_t *r, const poly *a);
void polyeta_unpack(poly *r, const uint8_t *a);

void polyz_pack(uint8_t *r, const poly *a);
void polyz_unpack(poly *r, const uint8_t *a);

#endif
