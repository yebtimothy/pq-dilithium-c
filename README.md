# pq-dilithium-c

A zero-dependency, optimized C implementation of the NIST **ML-DSA-65** (Dilithium3) digital signature scheme.

## Features

- **Zero External Dependencies**: Includes self-contained FIPS 202 (Keccak/SHAKE) implementation.
- **Optimized Arithmetic**: Custom Number Theoretic Transform (NTT) and reduction routines tailored for the Dilithium prime field.
- **No Dynamic Allocation**: Safe for embedded, systems-level, and bare-metal execution environments.
- **Verification Integrity**: Includes standard resistance checks against message and signature tampering.

## File Structure

- [`include/`](include/): Public API definitions (`api.h`) and algorithm parameters (`params.h`).
- [`src/`](src/): Implementation of Keccak/SHAKE (`fips202.c`), field arithmetic (`reduce.c`), polynomial transforms (`ntt.c`, `poly.c`, `polyvec.c`), serialization (`packing.c`), and cryptographic signature/verification (`sign.c`).
- [`tests/`](tests/): Test suite for correctness validation and benchmarking harness (`test_dilithium.c`).

## Build & Test

Ensure you have a C compiler (`gcc` or `clang`) and `make` installed.

### Build the library and test binary:
```bash
make
```

### Run correctness tests and benchmarks:
```bash
./test_dilithium
```

### Clean build artifacts:
```bash
make clean
```

## Benchmarks

Typical results on modern hardware (single core, `-O3 -march=native`):

- **Key Generation**: ~0.37 ms
- **Signing**: ~0.93 ms
- **Verification**: ~0.28 ms
