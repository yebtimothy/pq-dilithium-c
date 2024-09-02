#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "../include/api.h"

#define ITERATIONS 100

static inline uint64_t get_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

int main(void) {
  uint8_t pk[PQC_DILITHIUM_PUBLICKEYBYTES];
  uint8_t sk[PQC_DILITHIUM_SECRETKEYBYTES];
  uint8_t sig[PQC_DILITHIUM_BYTES];
  size_t siglen;
  
  const uint8_t msg[] = "This is a test message for post-quantum signature verification using Dilithium.";
  size_t msglen = sizeof(msg);
  
  printf("Starting pq-dilithium-c test harness...\n\n");
  
  // 1. Correctness Test
  printf("[TEST] Running KeyPair generation...\n");
  if (pqc_dilithium_keypair(pk, sk) != 0) {
    printf("[FAIL] KeyPair generation failed.\n");
    return -1;
  }
  printf("[PASS] KeyPair generation succeeded.\n");
  printf("       Public Key bytes: %d\n", PQC_DILITHIUM_PUBLICKEYBYTES);
  printf("       Secret Key bytes: %d\n", PQC_DILITHIUM_SECRETKEYBYTES);
  
  printf("[TEST] Signing message...\n");
  if (pqc_dilithium_sign(sig, &siglen, msg, msglen, sk) != 0) {
    printf("[FAIL] Signing failed.\n");
    return -1;
  }
  printf("[PASS] Signing succeeded.\n");
  printf("       Signature bytes:  %zu (Limit: %d)\n", siglen, PQC_DILITHIUM_BYTES);
  
  printf("[TEST] Verifying signature...\n");
  if (pqc_dilithium_verify(sig, siglen, msg, msglen, pk) != 0) {
    printf("[FAIL] Signature verification failed.\n");
    return -1;
  }
  printf("[PASS] Signature verification succeeded.\n");
  
  // 2. Tampering test (Modified Message)
  printf("[TEST] Verifying invalid message signature...\n");
  uint8_t bad_msg[sizeof(msg)];
  memcpy(bad_msg, msg, msglen);
  bad_msg[0] ^= 0x01; // Corrupt a bit
  if (pqc_dilithium_verify(sig, siglen, bad_msg, msglen, pk) == 0) {
    printf("[FAIL] Signature verified successfully for modified message (should fail).\n");
    return -1;
  }
  printf("[PASS] Modified message signature correctly rejected.\n");
  
  // 3. Tampering test (Modified Signature)
  printf("[TEST] Verifying invalid signature signature...\n");
  uint8_t bad_sig[PQC_DILITHIUM_BYTES];
  memcpy(bad_sig, sig, siglen);
  bad_sig[50] ^= 0x01; // Corrupt signature byte
  if (pqc_dilithium_verify(bad_sig, siglen, msg, msglen, pk) == 0) {
    printf("[FAIL] Corrupted signature verified successfully (should fail).\n");
    return -1;
  }
  printf("[PASS] Corrupted signature correctly rejected.\n");

  // 4. Benchmarking
  printf("\n[BENCHMARK] Running %d iterations...\n", ITERATIONS);
  
  uint64_t start, end;
  uint64_t keygen_total = 0;
  uint64_t sign_total = 0;
  uint64_t verify_total = 0;
  
  for (int i = 0; i < ITERATIONS; i++) {
    start = get_ns();
    pqc_dilithium_keypair(pk, sk);
    end = get_ns();
    keygen_total += (end - start);
    
    start = get_ns();
    pqc_dilithium_sign(sig, &siglen, msg, msglen, sk);
    end = get_ns();
    sign_total += (end - start);
    
    start = get_ns();
    pqc_dilithium_verify(sig, siglen, msg, msglen, pk);
    end = get_ns();
    verify_total += (end - start);
  }
  
  printf("  Key Generation:  avg %.2f ms\n", (double)keygen_total / ITERATIONS / 1000000.0);
  printf("  Signing:         avg %.2f ms\n", (double)sign_total / ITERATIONS / 1000000.0);
  printf("  Verification:    avg %.2f ms\n", (double)verify_total / ITERATIONS / 1000000.0);
  
  printf("\nAll tests completed successfully!\n");
  return 0;
}
