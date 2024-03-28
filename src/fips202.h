#ifndef FIPS202_H
#define FIPS202_H

#include <stddef.h>
#include <stdint.h>

#define SHAKE128_RATE 168
#define SHAKE256_RATE 136

typedef struct {
  uint64_t s[25];
  unsigned int pos;
} shake128state;

typedef struct {
  uint64_t s[25];
  unsigned int pos;
} shake256state;

void shake128_init(shake128state *state);
void shake128_absorb(shake128state *state, const uint8_t *input, size_t inlen);
void shake128_finalize(shake128state *state);
void shake128_squeezerx(shake128state *state, uint8_t *output, size_t outlen);

void shake256_init(shake256state *state);
void shake256_absorb(shake256state *state, const uint8_t *input, size_t inlen);
void shake256_finalize(shake256state *state);
void shake256_squeezerx(shake256state *state, uint8_t *output, size_t outlen);

void shake128(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen);
void shake256(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen);

#endif
