#ifndef API_H
#define API_H

#include <stddef.h>
#include <stdint.h>
#include "params.h"

// Public Key size: 1952 bytes
#define PQC_DILITHIUM_PUBLICKEYBYTES PUBLICKEYBYTES
// Private Key size: 4016 bytes (approx)
#define PQC_DILITHIUM_SECRETKEYBYTES PRIVATEKEYBYTES
// Signature size: 3293 bytes (approx)
#define PQC_DILITHIUM_BYTES SIGNATUREBYTES

/**
 * Generate a new Dilithium keypair.
 * pk: output buffer for the public key (PQC_DILITHIUM_PUBLICKEYBYTES bytes)
 * sk: output buffer for the secret key (PQC_DILITHIUM_SECRETKEYBYTES bytes)
 * Returns 0 on success.
 */
int pqc_dilithium_keypair(uint8_t *pk, uint8_t *sk);

/**
 * Sign a message.
 * sig: output signature buffer (PQC_DILITHIUM_BYTES bytes)
 * siglen: output pointer to write actual signature length
 * msg: input message buffer
 * msglen: message length in bytes
 * sk: secret key buffer (PQC_DILITHIUM_SECRETKEYBYTES bytes)
 * Returns 0 on success.
 */
int pqc_dilithium_sign(uint8_t *sig, size_t *siglen,
                       const uint8_t *msg, size_t msglen,
                       const uint8_t *sk);

/**
 * Verify a signature.
 * sig: input signature buffer (PQC_DILITHIUM_BYTES bytes)
 * siglen: signature length in bytes
 * msg: input message buffer
 * msglen: message length in bytes
 * pk: public key buffer (PQC_DILITHIUM_PUBLICKEYBYTES bytes)
 * Returns 0 if verification succeeds, negative value on failure.
 */
int pqc_dilithium_verify(const uint8_t *sig, size_t siglen,
                         const uint8_t *msg, size_t msglen,
                         const uint8_t *pk);

#endif
