# Best Practices

These tips capture the recurring setup and review habits that keep the engine deterministic and healthy.

## Build and toolchain

- Use the pinned CMake/Ninja toolchain and a modern C++20 compiler; keep `CMAKE_BUILD_TYPE` consistent across runs to preserve determinism.
- Prefer Release builds (`-DCMAKE_BUILD_TYPE=Release`) when validating latency or throughput; add `-DENABLE_SANITIZERS=ON` on Debug builds to surface UB early.
- Regenerate `build/compile_commands.json` when the build directory changes so IDEs and static analyzers stay in sync.
- Run `clang-format` before committing to avoid noisy diffs.

## Determinism and benchmarking

- Use the golden fixtures under `data/golden/` to confirm byte-for-byte stability before broader changes.
- For performance regressions, run `scripts/verify_bench.py` so p50/p95/p99 thresholds remain enforced.
- Keep the environment stable (timezone, locale, compiler version) across benchmark runs to avoid artificial drift.

## Testing and CI hygiene

- Execute the determinism and smoke workflows locally when possible before pushing; they mirror the CI gates.
- Review `artifacts/bench.jsonl` and `artifacts/metrics.prom` outputs after runs to catch tail spikes early.
- Avoid introducing non-deterministic constructs (unordered containers without stable iteration, random seeds without explicit values).

## Documentation and safety

- Document new gate policies or replay behaviors in `docs/` so CI reviewers can trace intent.
- Keep secrets out of the repo; the Detect Secrets workflow will fail builds when incidental tokens are committed.
