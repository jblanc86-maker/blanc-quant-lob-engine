# Reproduce in <10 minutes
1) `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build -j`
2) `make verify` → determinism PASS ✔︎
3) `make bench`  → prints p50/p95/p99; see artifacts/bench.jsonl
4) `make release`→ tarball with replay + golden + verify.sh

## macOS deterministic toolchain

Use the Apple clang preset to avoid clang-cl selection:

1) `cmake --preset macos-appleclang`
2) `cmake --build --preset macos-appleclang`

## Long-horizon stability

For 24h evidence runs (self-hosted):

1) `scripts/long_horizon_stability.sh` (see `docs/LONG_HORIZON_STABILITY.md`)
2) Upload `artifacts/stability/` as the release evidence bundle
