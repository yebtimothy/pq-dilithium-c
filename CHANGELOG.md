# Changelog

All notable changes to `pq-dilithium-c` are documented here.
Versioning follows [Semantic Versioning](https://semver.org/) while the API is pre-1.0.

## [0.1.0] - 2025-07-15

Initial tagged release.

- ML-DSA-65 (Dilithium3) key generation, signing, and verification
- Zero-dependency FIPS 202 (Keccak/SHAKE) implementation
- Shared `dilithium_commit()` path used by signing and `pq-pow-dilithium`
- Static library build via `make`; correctness tests and benchmarks in `tests/`

Dependents tested against this tag: `pq-pow-dilithium`, `pq-consensus-pow`,
`hybrid-hsm-enclave`, `pq-hd-wallet`.

[0.1.0]: https://github.com/yebtimothy/pq-dilithium-c/releases/tag/v0.1.0
