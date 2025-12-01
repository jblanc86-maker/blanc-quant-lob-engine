# PE Diligence — Technology Summary (1-slide)

Blanc LOB Engine — quick tech snapshot to support private-equity diligence:

TL;DR

- Functionally deterministic C++20 Level-2 order-book replay and benchmarking framework.
- Built with CMake + Ninja; small dependency surface (Boost program_options, standard C++) and a minimal CI for reproducible builds & artifacts.
- Golden-state verification via FNV digest + telemetry-driven book snapshot test to detect functional (state) regressions.

Core capabilities

- Deterministic replay: FNV digest check for full replay equality across runs.
- Per-run observability: bench.jsonl (per-run JSONL), metrics.prom (Prometheus textfile), PR/CI bench outputs.
- Breaker gates: runtime safety gates (Fuse/Local/Feeder/Main/Kill) with telemetry & publish flag.
- CPU pin support for reproducible latency measurements (Linux only; no-op on macOS).

Representative metrics (local lab runs)

- p50 ≈ 10ms; p95 ≈ 10.5–13ms depending on load & pinning; p99 ≈ same as p95 in these micro-bench samples.
- Observed: CPU pinning reduced p50 mean and reduced p95 variance in local tests (5× runs). Larger scale & more hosts needed for statistical confidence.

Test & CI maturity

- Core tests: unit test for breaker and telemetry; golden-state FNV comparison; snapshot-based golden test for telemetry/book state.
- CI: per-run ART_DIR artifacts, nightly determinism workflow, preview PR workflow to pin GitHub actions safely.

Risks & mitigations

- Non-determinism due to hardware scheduling: mitigate with CPU pinning and tighter worker-thread affinity (Phase 4).
- Private dependencies or pinned actions: pin-by-SHA + preview PR to prevent accidental automation drift.
- Test coverage: currently telemetry-based snapshot (work in Phase 2 to capture full book serialization and deep state diffs).

IP & licensing

- License: BUSL-1.1 (research & evaluation ok; commercial use requires explicit commercial license).
- Patent: breaker-style gate features are patent-noted (check legal & IP note in repo).

PE Diligence asks (recommended)

1) Re-run core benchmarks on representative hardware (two Linux machines, same kernel & same CPU families). Provide a CI job or reproduce locally.
2) Run nightly snapshot tests on multiple runners and review artifacts. Add baseline comparisons & thresholds.
3) Perform a code-security and dependency audit; confirm license compatibility.
4) Consider extending the snapshot framework to capture full OrderBook serialization (Phase 2) for stronger functional guarantees.

Appendix — Where to look

- Build & tests: `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release` + `ctest --output-on-failure`.
- Determinism: `scripts/verify_golden.sh` & CI `determinism.yml`.
- Pin preview: `.github/workflows/pin-actions-preview.yml`.
- Snapshot test: `tests/test_book_snapshot.cpp` and goldens under `tests/golden/`.
