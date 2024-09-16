#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../src/ntt.c"
#include "../src/reduce.c"

// invntt without the final f multiplication
void invntt_butterfly_only(int32_t a[256]) {
  unsigned int len, start, j, k;
  int32_t zeta, t;

  k = 256;
  for (len = 1; len < 256; len <<= 1) {
    for (start = 0; start < 256; start = j + len) {
      zeta = -zetas[--k];
      for (j = start; j < start + len; ++j) {
        t = a[j];
        a[j] = t + a[j + len];
        a[j + len] = t - a[j + len];
        a[j + len] = montgomery_reduce((int64_t)zeta * a[j + len]);
      }
    }
  }
}

int main(void) {
  int32_t x[256];
  int32_t x_ntt[256];
  int32_t x_recon[256];
  
  srand(42);
  for (int i = 0; i < 256; i++) {
    x[i] = (rand() % 201) - 100;
    x_ntt[i] = x[i];
  }
  
  ntt(x_ntt);
  
  for (int i = 0; i < 256; i++) {
    x_recon[i] = x_ntt[i];
  }
  
  invntt_butterfly_only(x_recon);
  
  // Search for f
  printf("Searching for correct scaling factor f...\n");
  int32_t found_f = -1;
  for (int32_t test_f = 0; test_f < DILITHIUM_Q; test_f++) {
    int32_t got = freeze32(montgomery_reduce((int64_t)test_f * x_recon[0]));
    int32_t expected = freeze32(x[0]);
    if (got == expected) {
      // Verify for all other coefficients
      int match = 1;
      for (int i = 1; i < 256; i++) {
        if (freeze32(montgomery_reduce((int64_t)test_f * x_recon[i])) != freeze32(x[i])) {
          match = 0;
          break;
        }
      }
      if (match) {
        found_f = test_f;
        break;
      }
    }
  }
  
  if (found_f != -1) {
    printf("SUCCESS! Found scaling factor f = %d\n", found_f);
  } else {
    printf("FAILED to find any valid scaling factor f. There might be a butterfly sequence mismatch.\n");
  }
  
  return 0;
}
