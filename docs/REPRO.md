# Reproduce in <10 minutes
1) `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build -j`
2) `make verify` → determinism PASS ✔︎
3) `make bench`  → prints p50/p95/p99; see artifacts/bench.jsonl
4) `make release`→ tarball with replay + golden + verify.sh
