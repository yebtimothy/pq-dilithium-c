#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "fips202.h"

#define NROUNDS 24

static const uint64_t KeccakF1600RoundConstants[NROUNDS] = {
  0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL,
  0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
  0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
  0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
  0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL,
  0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
  0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL,
  0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

#define ROL(a, offset) ((a << offset) ^ (a >> (64 - offset)))

static void keccak_f1600(uint64_t state[25]) {
  int round;
  uint64_t Aba, Abe, Abi, Abo, Abu;
  uint64_t Aga, Age, Agi, Ago, Agu;
  uint64_t Aka, Ake, Aki, Ako, Aku;
  uint64_t Ama, Ame, Ami, Amo, Amu;
  uint64_t Asa, Ase, Asi, Aso, Asu;
  uint64_t Bca, Bce, Bci, Bco, Bcu;
  uint64_t Da, De, Di, Do, Du;
  uint64_t Eba, Ebe, Ebi, Ebo, Ebu;
  uint64_t Ega, Ege, Egi, Ego, Egu;
  uint64_t Eka, Eke, Eki, Eko, Eku;
  uint64_t Ema, Eme, Emi, Emo, Emu;
  uint64_t Esa, Ese, Esi, Eso, Esu;

  Aba = state[0];  Abe = state[1];  Abi = state[2];  Abo = state[3];  Abu = state[4];
  Aga = state[5];  Age = state[6];  Agi = state[7];  Ago = state[8];  Agu = state[9];
  Aka = state[10]; Ake = state[11]; Aki = state[12]; Ako = state[13]; Aku = state[14];
  Ama = state[15]; Ame = state[16]; Ami = state[17]; Amo = state[18]; Amu = state[19];
  Asa = state[20]; Ase = state[21]; Asi = state[22]; Aso = state[23]; Asu = state[24];

  for (round = 0; round < NROUNDS; round += 2) {
    // Round 1
    Bca = Aba ^ Aga ^ Aka ^ Ama ^ Asa;
    Bce = Abe ^ Age ^ Ake ^ Ame ^ Ase;
    Bci = Abi ^ Agi ^ Aki ^ Ami ^ Asi;
    Bco = Abo ^ Ago ^ Ako ^ Amo ^ Aso;
    Bcu = Abu ^ Agu ^ Aku ^ Amu ^ Asu;

    Da = Bcu ^ ROL(Bce, 1);
    De = Bca ^ ROL(Bci, 1);
    Di = Bce ^ ROL(Bco, 1);
    Do = Bci ^ ROL(Bcu, 1);
    Du = Bco ^ ROL(Bca, 1);

    Aba ^= Da; Abe ^= De; Abi ^= Di; Abo ^= Do; Abu ^= Du;
    Aga ^= Da; Age ^= De; Agi ^= Di; Ago ^= Do; Agu ^= Du;
    Aka ^= Da; Ake ^= De; Aki ^= Di; Ako ^= Do; Aku ^= Du;
    Ama ^= Da; Ame ^= De; Ami ^= Di; Amo ^= Do; Amu ^= Du;
    Asa ^= Da; Ase ^= De; Asi ^= Di; Aso ^= Do; Asu ^= Du;

    Bca = Aba;
    Bce = ROL(Age, 44);
    Bci = ROL(Aki, 43);
    Bco = ROL(Amo, 21);
    Bcu = ROL(Asu, 14);
    Eba = Bca ^ ((~Bce) & Bci); Eba ^= KeccakF1600RoundConstants[round];
    Ebe = Bce ^ ((~Bci) & Bco);
    Ebi = Bci ^ ((~Bco) & Bcu);
    Ebo = Bco ^ ((~Bcu) & Bca);
    Ebu = Bcu ^ ((~Bca) & Bce);

    Bca = ROL(Abe, 1);
    Bce = ROL(Agi, 6);
    Bci = ROL(Ako, 25);
    Bco = ROL(Amu, 8);
    Bcu = ROL(Asa, 18);
    Ega = Bca ^ ((~Bce) & Bci);
    Ege = Bce ^ ((~Bci) & Bco);
    Egi = Bci ^ ((~Bco) & Bcu);
    Ego = Bco ^ ((~Bcu) & Bca);
    Egu = Bcu ^ ((~Bca) & Bce);

    Bca = ROL(Abi, 62);
    Bce = ROL(Ago, 55);
    Bci = ROL(Aku, 39);
    Bco = ROL(Ama, 41);
    Bcu = ROL(Ase, 2);
    Eka = Bca ^ ((~Bce) & Bci);
    Eke = Bce ^ ((~Bci) & Bco);
    Eki = Bci ^ ((~Bco) & Bcu);
    Eko = Bco ^ ((~Bcu) & Bca);
    Eku = Bcu ^ ((~Bca) & Bce);

    Bca = ROL(Abo, 28);
    Bce = ROL(Agu, 20);
    Bci = ROL(Aka, 3);
    Bco = ROL(Ame, 45);
    Bcu = ROL(Asi, 61);
    Ema = Bca ^ ((~Bce) & Bci);
    Eme = Bce ^ ((~Bci) & Bco);
    Emi = Bci ^ ((~Bco) & Bcu);
    Emo = Bco ^ ((~Bcu) & Bca);
    Emu = Bcu ^ ((~Bca) & Bce);

    Bca = ROL(Abu, 27);
    Bce = ROL(Aga, 36);
    Bci = ROL(Ake, 10);
    Bco = ROL(Ami, 15);
    Bcu = ROL(Aso, 56);
    Esa = Bca ^ ((~Bce) & Bci);
    Ese = Bce ^ ((~Bci) & Bco);
    Esi = Bci ^ ((~Bco) & Bcu);
    Eso = Bco ^ ((~Bcu) & Bca);
    Esu = Bcu ^ ((~Bca) & Bce);

    // Round 2
    Bca = Eba ^ Ega ^ Eka ^ Ema ^ Esa;
    Bce = Ebe ^ Ege ^ Eke ^ Eme ^ Ese;
    Bci = Ebi ^ Egi ^ Eki ^ Emi ^ Esi;
    Bco = Ebo ^ Ego ^ Eko ^ Emo ^ Eso;
    Bcu = Ebu ^ Egu ^ Eku ^ Emu ^ Esu;

    Da = Bcu ^ ROL(Bce, 1);
    De = Bca ^ ROL(Bci, 1);
    Di = Bce ^ ROL(Bco, 1);
    Do = Bci ^ ROL(Bcu, 1);
    Du = Bco ^ ROL(Bca, 1);

    Eba ^= Da; Ebe ^= De; Ebi ^= Di; Ebo ^= Do; Ebu ^= Du;
    Ega ^= Da; Ege ^= De; Egi ^= Di; Ego ^= Do; Egu ^= Du;
    Eka ^= Da; Eke ^= De; Eki ^= Di; Eko ^= Do; Eku ^= Du;
    Ema ^= Da; Eme ^= De; Emi ^= Di; Emo ^= Do; Emu ^= Du;
    Esa ^= Da; Ese ^= De; Esi ^= Di; Eso ^= Do; Esu ^= Du;

    Bca = Eba;
    Bce = ROL(Ege, 44);
    Bci = ROL(Eki, 43);
    Bco = ROL(Emo, 21);
    Bcu = ROL(Esu, 14);
    Aba = Bca ^ ((~Bce) & Bci); Aba ^= KeccakF1600RoundConstants[round + 1];
    Abe = Bce ^ ((~Bci) & Bco);
    Abi = Bci ^ ((~Bco) & Bcu);
    Abo = Bco ^ ((~Bcu) & Bca);
    Abu = Bcu ^ ((~Bca) & Bce);

    Bca = ROL(Ebe, 1);
    Bce = ROL(Egi, 6);
    Bci = ROL(Eko, 25);
    Bco = ROL(Emu, 8);
    Bcu = ROL(Esa, 18);
    Aga = Bca ^ ((~Bce) & Bci);
    Age = Bce ^ ((~Bci) & Bco);
    Agi = Bci ^ ((~Bco) & Bcu);
    Ago = Bco ^ ((~Bcu) & Bca);
    Agu = Bcu ^ ((~Bca) & Bce);

    Bca = ROL(Ebi, 62);
    Bce = ROL(Ego, 55);
    Bci = ROL(Eku, 39);
    Bco = ROL(Ema, 41);
    Bcu = ROL(Ese, 2);
    Aka = Bca ^ ((~Bce) & Bci);
    Ake = Bce ^ ((~Bci) & Bco);
    Aki = Bci ^ ((~Bco) & Bcu);
    Ako = Bco ^ ((~Bcu) & Bca);
    Aku = Bcu ^ ((~Bca) & Bce);

    Bca = ROL(Ebo, 28);
    Bce = ROL(Egu, 20);
    Bci = ROL(Eka, 3);
    Bco = ROL(Eme, 45);
    Bcu = ROL(Esi, 61);
    Ama = Bca ^ ((~Bce) & Bci);
    Ame = Bce ^ ((~Bci) & Bco);
    Ami = Bci ^ ((~Bco) & Bcu);
    Amo = Bco ^ ((~Bcu) & Bca);
    Amu = Bcu ^ ((~Bca) & Bce);

    Bca = ROL(Ebu, 27);
    Bce = ROL(Ega, 36);
    Bci = ROL(Eke, 10);
    Bco = ROL(Emi, 15);
    Bcu = ROL(Eso, 56);
    Asa = Bca ^ ((~Bce) & Bci);
    Ase = Bce ^ ((~Bci) & Bco);
    Asi = Bci ^ ((~Bco) & Bcu);
    Aso = Bco ^ ((~Bcu) & Bca);
    Asu = Bcu ^ ((~Bca) & Bce);
  }

  state[0] = Aba;  state[1] = Abe;  state[2] = Abi;  state[3] = Abo;  state[4] = Abu;
  state[5] = Aga;  state[6] = Age;  state[7] = Agi;  state[8] = Ago;  state[9] = Agu;
  state[10] = Aka; state[11] = Ake; state[12] = Aki; state[13] = Ako; state[14] = Aku;
  state[15] = Ama; state[16] = Ame; state[17] = Ami; state[18] = Amo; state[19] = Amu;
  state[20] = Asa; state[21] = Ase; state[22] = Asi; state[23] = Aso; state[24] = Asu;
}

static void shake_init(uint64_t s[25]) {
  memset(s, 0, 25 * sizeof(uint64_t));
}

static void shake_absorb(uint64_t s[25], unsigned int *pos, unsigned int rate, const uint8_t *input, size_t inlen) {
  size_t i;
  unsigned int p = *pos;
  
  for (i = 0; i < inlen; i++) {
    s[p / 8] ^= ((uint64_t)input[i]) << (8 * (p % 8));
    p++;
    if (p == rate) {
      keccak_f1600(s);
      p = 0;
    }
  }
  *pos = p;
}

static void shake_finalize(uint64_t s[25], unsigned int pos, unsigned int rate, uint8_t domain_separator) {
  s[pos / 8] ^= ((uint64_t)domain_separator) << (8 * (pos % 8));
  s[(rate - 1) / 8] ^= 0x8000000000000000ULL;
  keccak_f1600(s);
}

static void shake_squeeze(uint64_t s[25], unsigned int *pos, unsigned int rate, uint8_t *output, size_t outlen) {
  size_t i;
  unsigned int p = *pos;
  
  for (i = 0; i < outlen; i++) {
    output[i] = (s[p / 8] >> (8 * (p % 8))) & 0xFF;
    p++;
    if (p == rate) {
      keccak_f1600(s);
      p = 0;
    }
  }
  *pos = p;
}

void shake128_init(shake128state *state) {
  shake_init(state->s);
  state->pos = 0;
}

void shake128_absorb(shake128state *state, const uint8_t *input, size_t inlen) {
  shake_absorb(state->s, &state->pos, SHAKE128_RATE, input, inlen);
}

void shake128_finalize(shake128state *state) {
  shake_finalize(state->s, state->pos, SHAKE128_RATE, 0x1F);
  state->pos = 0;
}

void shake128_squeezerx(shake128state *state, uint8_t *output, size_t outlen) {
  shake_squeeze(state->s, &state->pos, SHAKE128_RATE, output, outlen);
}

void shake256_init(shake256state *state) {
  shake_init(state->s);
  state->pos = 0;
}

void shake256_absorb(shake256state *state, const uint8_t *input, size_t inlen) {
  shake_absorb(state->s, &state->pos, SHAKE256_RATE, input, inlen);
}

void shake256_finalize(shake256state *state) {
  shake_finalize(state->s, state->pos, SHAKE256_RATE, 0x1F);
  state->pos = 0;
}

void shake256_squeezerx(shake256state *state, uint8_t *output, size_t outlen) {
  shake_squeeze(state->s, &state->pos, SHAKE256_RATE, output, outlen);
}

void shake128(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen) {
  shake128state state;
  shake128_init(&state);
  shake128_absorb(&state, in, inlen);
  shake128_finalize(&state);
  shake128_squeezerx(&state, out, outlen);
}

void shake256(uint8_t *out, size_t outlen, const uint8_t *in, size_t inlen) {
  shake256state state;
  shake256_init(&state);
  shake256_absorb(&state, in, inlen);
  shake256_finalize(&state);
  shake256_squeezerx(&state, out, outlen);
}
