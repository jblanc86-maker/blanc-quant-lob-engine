## Blanc Quant LOB Engine (BQL Engine)

<!-- DO NOT REMOVE: Visitors badge for repo analytics -->

### Status Badges

[![p50/p95/p99](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/jblanc86-maker/blanc-quant-lob-engine/main/badges/performance_extra.json)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/ci.yml)
[![Reproducible](https://img.shields.io/badge/Reproducible-Builds-4caf50.svg)](docs/REPRO.md)
[![Golden-state Deterministic Replay](https://img.shields.io/badge/Golden--state%20Deterministic%20Replay-brightgreen.svg)](docs/gates.md)
[![Determinism](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism.yml)
[![Determinism Compiler Matrix](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism_compiler_matrix.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism_compiler_matrix.yml)
[![Determinism Kernel Matrix](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism_kernel_matrix.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism_kernel_matrix.yml)
[![Determinism Affinity + Kernel](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism_affinity_kernel.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism_affinity_kernel.yml)
[![Gate Replay Determinism](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism_gate_replay.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/determinism_gate_replay.yml)
[![Verify Bench](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/verify-bench.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/verify-bench.yml)
[![CI](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/ci.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/ci.yml)
[![Docs Lint](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/docs-lint.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/docs-lint.yml)
[![CodeQL](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/codeql.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/codeql.yml)
[![Container Scan (Trivy v0.67.2)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/container-scan.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/container-scan.yml)
[![Detect Secrets](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/secrets-scan.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/secrets-scan.yml)
[![Smoke SITREP](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/smoke-sitrep.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/smoke-sitrep.yml)
[![Snapshot Nightly](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/snapshot-nightly.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/snapshot-nightly.yml)
[![Workflow Usage Report](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/workflow-usage-report.yml/badge.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/actions/workflows/workflow-usage-report.yml)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/pulls)
[![GitHub Last Commit](https://img.shields.io/github/last-commit/jblanc86-maker/blanc-quant-lob-engine)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/commits/main)
[![GitHub Release](https://img.shields.io/github/v/release/jblanc86-maker/blanc-quant-lob-engine?include_prereleases)](https://github.com/jblanc86-maker/blanc-quant-lob-engine/releases)
[![CMake + Ninja](https://img.shields.io/badge/build-CMake%20%2B%20Ninja-7952b3.svg)](https://ninja-build.org/)
[![Tail Matrix Evidence](https://img.shields.io/badge/Tail%20Matrix-100x%2F10x%2F10x-blue)](docs/gates.md)
[![Code Size](https://img.shields.io/github/languages/code-size/jblanc86-maker/blanc-quant-lob-engine)](https://github.com/jblanc86-maker/blanc-quant-lob-engine)
[![Top Language](https://img.shields.io/github/languages/top/jblanc86-maker/blanc-quant-lob-engine?logo=cplusplus)](https://github.com/jblanc86-maker/blanc-quant-lob-engine)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-blue.svg)](https://cmake.org/)
[![Docker](https://img.shields.io/badge/Docker-Enabled-2496ED.svg?logo=docker&logoColor=white)](https://hub.docker.com/)
[![License: BSL-1.1](https://img.shields.io/badge/License-BSL--1.1-blue.svg)](LICENSE)
![Visitors](https://visitor-badge.laobi.icu/badge?page_id=jblanc86-maker.blanc-quant-lob-engine)

### Performance: Current State vs. Future Targets

| Metric Tier                                                | Current (Jan 2026)                                                          | Target (vNext)                                                                     | Status                  |
| ---------------------------------------------------------- | --------------------------------------------------------------------------- | ---------------------------------------------------------------------------------- | ----------------------- |
| **Tier A: Match-only**<br/>(Core engine speed)             | p50: 1.25μs<br/>p95: 3.29μs<br/>p99: 5.67μs                                 | p50: 100–300μs<br/>p95: 200–600μs<br/>p99: 300–900μs                               | ✅ **EXCEEDS TARGET**   |
| **Tier B: In-process Wire-to-Wire**<br/>(No network/disk)  | Not yet separated                                                           | p50: 0.5–1.5ms<br/>p95: 1–3ms<br/>p99: 2–5ms                                       | 🎯 Planned              |
| **Tier C: Proof Pipeline**<br/>(Full deterministic replay) | p50: ~16ms<br/>p95: ~18ms<br/>p99: ~20ms<br/>p99.9: ~22ms<br/>p99.99: ~24ms | p50: 2–6ms<br/>p95: 4–10ms<br/>p99: 6–15ms<br/>p99.9: ≤3× p99<br/>p99.99: advisory | 🚧 Optimization Phase 2 |
| **Throughput**                                             | 1M events/sec                                                               | 1–5M ops/sec                                                                       | ✅ Baseline Established |
| **Deterministic Replay**                                   | ✅ Verified (100% digest consistency)                                       | ✅ Enhanced with SCM                                                               | ✅ Production Ready     |

> **Tail Latency Purity** — p99.9 and p99.99 are now measured on every run (≥1k samples for p99.9 and ≥10k
> samples for stable p99.99; 1M synthetic events = ~15,625 64-byte chunks). Runs emit `samples`,
> `p999_valid`, and `p9999_valid` to prevent under-sampled tails from being misinterpreted. p99.9 is
> gated at ≤ 3× the p99 budget; p99.99 is reported as an advisory metric. Gate tail-delta is
> independently validated by `tests/test_tail_latency.cpp`.

> Update

## Selective Coordination Mode (SCM): Smarter, Deterministic Protection

Selective Coordination Mode brings the “smallest breaker trips first” principle from power systems into trading engines.
Instead of halting everything when there’s a slowdown, the engine disables or sheds only the affected subsystem—keeping the rest running and making incident boundaries clean and replayable.

### How It Works

- **Zones:** The engine is divided into protection zones (core match, risk checks, telemetry, journaling, snapshotting, adapters).
- **Trip Ladder:** If a zone (like telemetry) gets slow, only that zone is tripped first. If the problem persists, the next zone up the ladder is tripped, and so on—up to a full halt as a last resort.
- **Coordination Curves:** Each zone has its own latency budget and trip logic (e.g., “if p99 latency is breached for M out of N events, trip this zone”).
- **Escalation & Recovery:** The system only escalates if the problem continues, and only recovers after a period of good performance—avoiding constant switching.
- **Deterministic Journal:** Every trip, recovery, and action is logged, so you can replay and audit exactly what happened, when, and why.

### Coordination Diagram

```text
┌────────────┐   trip   ┌──────────────┐   trip   ┌──────────────┐   trip   ┌────────────┐   trip   ┌───────┐
│ Telemetry  │ ───────▶ │ Journaling   │ ───────▶ │ Risk Checks  │ ───────▶ │ Core Match │ ───────▶ │ HALT  │
└────────────┘          └──────────────┘          └──────────────┘          └────────────┘          └───────┘
```

Each zone is defined in `config/coordination_zones.yaml`. You can also render a visual escalation diagram from this
config using `scripts/visualize_coordination.py`, which saves an image to `artifacts/coordination_diagram.png`.
For a text-only fallback diagram, see `docs/how-it-works-ascii.txt`.

### Enhanced Configuration

See [`config/coordination_zones.yaml`](config/coordination_zones.yaml) for full coordination configuration featuring:

- **Multi-dimensional latency budgets:** p50/p95/p99/p999/max enforcement per zone
- **Per-zone recovery criteria:** Independent recovery logic with digest validation
- **Coordination curves:** Exponential backoff, adaptive threshold, and sustained breach detection
- **Hysteresis protection:** Recovery multiplier (1.25x) prevents oscillation
- **Priority-based escalation:** Zones 1-7 with the "smallest breaker trips first" principle
- **Deterministic replay:** Event index matching + digest chain enforcement

**Documentation:**

- [Patent Claims Analysis](docs/PATENT_CLAIMS_COORDINATION.md) - Detailed claim-by-claim analysis
- [Configuration Enhancements](docs/COORDINATION_ENHANCEMENTS.md) - Technical implementation guide
- [Performance Roadmap](docs/PERFORMANCE_ROADMAP.md) - Optimization strategy

### CI Test & Visualization

```bash
# Run coordination test
python3 scripts/ci_test_coordination.py

# Generate coordination diagram
python3 scripts/visualize_coordination.py config/coordination_zones.yaml
```

CI pipeline validates that zone trips, escalation, and recovery work deterministically.

## How the Safety Controller Works (Breaker-Style State Machine)

The engine uses a safety controller, similar to a circuit breaker, to keep things running smoothly and protect against slowdowns.

### The Five Modes

- **NORMAL:** Everything is working as expected.
- **WARN:** The system notices things are starting to slow down and gets ready to take action.
- **DEGRADED:** The engine turns off or reduces extra features (like detailed logging) to stay fast.
- **SHED:** The engine stops doing anything non-essential to focus on core tasks.
- **HALT:** The engine stops completely to prevent bad data and saves everything needed for later review.

### How It Decides to Switch Modes

- If too many slow events happen in a row, or if the system is slow for a certain amount of time, it moves to a stricter mode.
- It only goes back to a better mode after things have been good for a while (not just one fast event), so it doesn’t keep switching back and forth.
  This approach keeps the engine reliable and makes sure you always have a record of what happened if something goes wrong.

## Concrete Mechanisms: Breaker-Style State Machine (Core Controller)

The Dynamic Execution Gates (DEG) are implemented as a canonical breaker-style state machine, a pattern widely recognized in high-reliability systems. This controller governs the engine’s operational mode in real time, based on observed latency and SLO compliance.

### States

- **NORMAL:** Expected operation; all features enabled.
- **WARN:** Early warning; initial breach signals detected, begin mitigation.
- **DEGRADED:** Optional work (e.g., telemetry, non-critical processing) is reduced to protect tail latency.
- **SHED:** Non-critical work is aggressively shed to maintain core SLOs.
- **HALT:** Fail-fast; stop processing and preserve a full incident artifact set for audit and replay.

### Transitions (with Hysteresis)

State transitions are triggered by:

- **Rolling breach count:** Number of SLO breaches in a sliding window.
- **Quantile exceedance:** p95/p99 latency exceeding budget for M of the last N events.
- **Sustained breach duration:** Latency above threshold for a continuous time window.
  **Hysteresis** is built in to prevent state flapping:
- Recovery to a lower-severity state (e.g., DEGRADED → WARN, WARN → NORMAL) requires K consecutive “good” windows (i.e., no breaches) before transitioning.
  This design ensures robust, predictable control over the engine’s behavior under stress, while preserving deterministic replay and auditability.

## Normalized Latency Metrics: Apples-to-Apples Comparison

Latency claims in the trading engine space are often not directly comparable. Many engines report only a single,
best-case number—sometimes just a single operation, or a per-hop handoff, rather than true end-to-end performance.
To provide full transparency and enable apples-to-apples comparison, Blanc LOB Engine publishes three distinct latency
metrics:

- **Match-only latency:** Time for a single match operation inside the engine’s core loop (no I/O, no telemetry, no replay logic).
- **In-process wire-to-wire latency:** Time from API call to match to response, excluding network but including all in-process steps (parsing, matching, response generation).
- **Replay pipeline latency:** Time from event ingestion through application, digest calculation, and telemetry/artifact generation (full replay with determinism proofs).

**Roadmap Timeline:**

- **Phase 1 (0–2 weeks):** Tier separation, zero-allocation hot loop
- **Phase 2 (2–6 weeks):** Book structure optimization, async artifact writing
- **Phase 3 (6–12 weeks):** Deterministic scaling, binary journaling
- **Phase 4 (12–24 weeks):** Advanced tuning, optional kernel-bypass

See [Performance Roadmap](docs/PERFORMANCE_ROADMAP.md) and [Performance Targets](docs/PERFORMANCE_TARGETS.md) for detailed optimization strategy.

---

## About Blanc LOB Engine

The Blanc LOB Engine (BQL) is a high-performance, open-source limit order book
engine designed for financial market simulations and trading systems. It
provides robust features for order matching, market data replay, and telemetry,
making it ideal for students, hobbyists, and professionals exploring
algorithmic trading and market microstructure. It is a C++20 synthetic
microbenchmark focused on loop and memory performance—not a full market
replay/matching engine. It uses synthetic data, computes FNV-based digests for
basic validation, and emits limited telemetry.

BQL 2.0 (Patent-Pending) is the production-shaped system: real ITCH
market-data replay + deterministic order book matching, a per-event binary
audit journal (replayable proof), CI-enforced performance contracts
(p50/p95/p99/p999 + stability), and coordinated deterministic protection
(DEG/DSC/RSC/CDP/DEC/DPL/DEM). It also introduces canonical symbol IDs to
guarantee order/shard-invariant aggregate verification (Phase 6.2).

For inquiries related to trading applications or professional use cases, please feel free to reach out.

## Deterministic C++20 Limit Order Book (LOB) Replay & Benchmarking Engine

The Blanc LOB Engine (BQL) is a high-performance, open-source limit order book engine designed for financial market
simulations and trading systems. It provides robust features for order matching, market data replay, and telemetry,
making it ideal for students, hobbyists, and professionals exploring algorithmic trading and market microstructure.
Inquire for FULL proprietary features (600+ unique clones as of 12/07/25).

- **Deterministic replay:** Byte-for-byte golden-state checks over ITCH binaries and synthetic bursts.
- **Patent-pending Dynamic Execution Gates (DEG):** Breaker-style gate policies wrap the datapath with explicit safety and tail-latency controls. (Open-source release includes the core breaker state machine; some advanced DEG features remain proprietary.)
- **Tail SLO enforcement:** `scripts/verify_bench.py` treats p50/p95/p99 budgets as release gates, not suggestions.
- **Structured observability:** Every run emits JSONL and Prometheus-compatible text files for diffing, dashboards, and CI.

If you care about _"can we replay this exactly, under load, and prove it didn't get slower or weirder at tails?"_

## Phase 6.2 — Canonical Symbol IDs (Patent Evidence)

Phase 6.2 introduces canonical symbol IDs to make multi-symbol aggregation deterministic across symbol order and shard count. Core files:

- `include/canonical_symbol.hpp` — FNV-1a based canonical ID + aggregate digest
- `include/multi_book_engine_v2.hpp` — Books keyed by canonical ID (not raw symbol)
- `tests/test_canonical_digest.cpp` — Patent evidence tests (order/shard invariance)

Quick run (build + evidence test):

1. Build the test binary

```bash
cmake -S . -B build
cmake --build build --target test_canonical_digest
```

1. Run the tests

```bash
./build/bin/test_canonical_digest
```

Expected: All tests PASS and print digest/evidence lines suitable for filing. A CI workflow also packages an evidence bundle with provenance; see `.github/workflows/canonical_ids_evidence.yml` and `scripts/package_canonical_ids_evidence.sh`. Full attorney-facing write-up: `docs/PATENT_EVIDENCE_CANONICAL_IDS.md`.

### Architecture Overview (ASCII)

```text
Binary capture -> replay harness -> orderbook -> detectors/breaker -> digest

Legend:
  - Binary capture (data/golden/itch_1m.bin)
  - Replay harness: C++20, cache-friendly
  - Order-book core: SoA
  - Detectors + Breaker gates: runtime safeguards
  - Digest: deterministic end-state hash
  - Latency metrics: p50/p90/p99
  - Publish mode: run summary and outputs

Flow:
  [data/golden/itch_1m.bin] -> [replay harness] -> [order-book core]
    -> [detectors + breaker gates] -> {digest, latency metrics, publish summary}

Artifacts:
  - artifacts/bench.jsonl   # bench per-run JSON lines
  - artifacts/metrics.prom  # prometheus textfile metrics
  - stdout run summary
```

For distributed shard merge specification and visual figure, see:

- Evidence narrative: `docs/PATENT_EVIDENCE_CANONICAL_IDS.md` (Deterministic Shard Merge Specification)
- Figure: `docs/images/shard_merge_flow.svg` (shards → union/sort → global fold)

### Deterministic Shard Merge (ASCII)

```text
Shards (N)                                 Merge & Fold
-----------------------                     ------------------------------
Shard 1: (id,dig) pairs  \                 Union all shard pairs
Shard 2: (id,dig) pairs   \   -->          Sort by canonical id (tiebreak)
...                        /                 Iterate mix(id,dig,version)
Shard N: (id,dig) pairs   /                 Root digest: CANON_V1
-----------------------                     ------------------------------

Ordering stability rules:
- Collisions: compare raw_symbol_bytes as tiebreak
- Shard-count change: union of (id,dig) pairs is identical
- Processing-order change: per-symbol digests stable; sort normalizes order
```

### Provenance Artifact Checklist (CI Evidence)

- Bench CSV: `artifacts/scaling/bench.csv`
  - Hash: `artifacts/scaling/bench.csv.sha256`
- Bench log: `artifacts/scaling/bench.log`
  - Hash: `artifacts/scaling/bench.log.sha256`
- Bench summary (JSON): `artifacts/scaling/bench_summary.json`
  - Hash: `artifacts/scaling/bench_summary.json.sha256`
- Plot image (optional): `artifacts/scaling/bench.png`
  - Hash: `artifacts/scaling/bench.png.sha256`
- System info (uname): `artifacts/system/uname.txt`
  - Hash: `artifacts/system/uname.txt.sha256`
- Compiler info (gcc): `artifacts/system/gcc_version.txt`
  - Hash: `artifacts/system/gcc_version.txt.sha256`
- Compiler info (clang): `artifacts/system/clang_version.txt`
  - Hash: `artifacts/system/clang_version.txt.sha256`
- Evidence doc (bundle copy): `docs/evidence/canonical_ids_v1/PATENT_EVIDENCE_CANONICAL_IDS.md`
  - Hash: `docs/evidence/canonical_ids_v1/PATENT_EVIDENCE_CANONICAL_IDS.md.sha256`

CI uploads these under the run’s artifacts; see the cross-platform workflow for exact archive paths.

## Deterministic Enforcement: How DEG Keeps Replay Exact

A major challenge in trading engines is that runtime controls (like circuit breakers or latency gates) typically destroy determinism—timing and state can diverge across runs or hardware. The Blanc LOB Engine’s Dynamic Execution Gates (DEG) solve this via a two-layer principle:

### A) Deterministic Outcome = (Event Stream + Gate Decisions)

- Evolution of order book is fully deterministic, given:
  - Input event stream (market data, orders)
  - Gate Decision Journal (GDJ): a per-event log of DEG state transitions and actions
- During replay, engine re-applies same gate decisions at same event indices, guaranteeing byte-identical final state.

### B) Auditable Artifacts: Storing “Why” and “What Happened”

- For each event, engine records:
  - Latency sample (measured duration for that event)
  - Gate state (e.g., NORMAL, WARN, DEGRADED, HALT)
  - Action vector (e.g., none, degrade, reroute, halt)
  - State digest (e.g., FNV-1a) after event application
- This produces a closed-loop proof: replaying same event stream and gate decisions yields same state transitions and final book state, verifiable by digest.

### Defensible Claim Language

> "A method for deterministic enforcement in a limit order book engine, comprising: (a) recording, for each event, input, gate state, action taken, and resulting state digest; (b) replaying event stream with same gate decisions to guarantee byte-identical outcomes; and (c) providing auditable artifacts for each event to prove determinism and correctness."

---

## What Makes This Innovative

### 1. Golden-State Deterministic Replay

- Guarantees byte-for-byte identical results across runs.
- FNV-1a digest verification: Every replay produces a cryptographic fingerprint of final order book state.
- Automated dual-run CI: GitHub Actions runs same input twice and fails if digests differ—catching non-determinism instantly.
- Environment normalization: Fixed timezone, locale, and compiler ensure reproducibility.

### 2. Integrated Determinism + Performance Testing

- Same workflow proves determinism and measures p50/p95/p99 tail latency.
- Release gates enforce SLO budgets: if p99 regresses, CI fails.
- Structured artifacts (`bench.jsonl`, `metrics.prom`) enable historical tracking and automated dashboards.

### 3. Dynamic Execution Gates (Patent-Pending)

- Breaker-style state machine (Fuse → Local → Feeder → Main → Kill).
- Preserves deterministic replay while containing pathological scenarios.
- Explicit publish control: corrupted runs are flagged, not silently trusted.

### 4. Telemetry-Driven Golden-State Validation

- Every run produces machine-readable, CI-auditable artifacts.
- Structured outputs: JSONL event logs + Prometheus textfiles.
- Release gates as code: `scripts/verify_bench.py` treats performance budgets as pass/fail gates.
- Artifact packaging: Automated artifact creation with provenance metadata.

### 5. Canonical Serialization for Order Books

- Structure-of-Arrays (SoA) layout for cache efficiency.
- Fixed iteration order regardless of insertion sequence.
- FNV-1a rolling hash captures exact state, not approximations.

## System architecture

```text
┌──────────────────────────────────────────────────────────────────────────────┐
│   QUANT LOB ENGINE — Deterministic Replay & Benchmark Harness                │
├──────────────────────────────────────────────────────────────────────────────┤
│ Inputs                              Core                           Outputs    │
│ ┌─────────────────┐        ┌──────────────────────────────┐      ┌───────────┐│
│ │ trace_loader    │───▲───▶│ Deterministic Replay         │───┬─▶│ Stdout    ││
│ │ (ITCH bin; CSV/ │   │    │ Scheduler (ST; MT optional)  │   │  │ summary   ││
│ │ PCAP→bin bridge)│   │    └──────────────┬───────────────┘   │  └───────────┘│
│ └─────────────────┘   │                   │                   │              │
│ ┌─────────────────┐   │                   │                   │  ┌───────────┐│
│ │ gen_synth       │───┘   Fault Injection / Gates (DEG‑compatible;         ││
│ │ (synthetic)     │           breaker‑style, optional)           └─▶│ Artifacts ││
│ └─────────────────┘                                     ▲            │ bench.jsonl│
│                                                         │            │ metrics.prom│
│                                         ┌──────────────────────────────┐└───────────┘│
│                                         │ Golden-state Checker         │◀──────┘
│                                         │ (byte-for-byte digest_fnv)   │
│                                         └──────────────────────────────┘
│                                                 │
│                                                 ▼
│ ┌──────────────────────────────┐     ┌──────────────────────────────┐
│ │ Benchmark Harness            │     │ Structured Observability     │
│ │ • msgs/s throughput          │     │ • JSONL event logs           │
│ │ • p50/p95/p99 latency        │     │ • Prometheus textfile        │
│ │ • config matrix sweeps       │     │ • CI artifacts (goldens)     │
│ └──────────────────────────────┘     └──────────────────────────────┘
└──────────────────────────────────────────────────────────────────────────────┘
```

### Flow summary

- ITCH binaries and synthetic `gen_synth` bursts feed a deterministic scheduler
  that enforces DEG-compatible gate policies before emitting telemetry.
- Golden digest checks ensure byte-for-byte stability, while bench harness
  sweeps configs to publish `bench.jsonl`, Prometheus textfiles, and CI-ready
  artifacts.
- Structured observability (JSONL + textfile) makes it easy to diff runs,
  enforce SLOs, and root-cause tail spikes.
- Dynamic Execution Gates (DEG) model tail behavior as first-class policy,
  making "breaker-style" protections and SLO checks part of engine instead
  of bolted-on monitoring.

### Classic HFT datapath

```text
┌────────────────────────────────────────────────────────────────────┐
│                    QUANT LOB ENGINE (HFT SYSTEM)                   │
├────────────────────────────────────────────────────────────────────┤
│  ITCH 5.0 parser  ──▶  L2/L3 order book (SoA) ──▶  Price levels    │
│            │                             │                        │
│            ▼                             ▼                        │
│      Dynamic Execution Gates (DEG) ──▶ Telemetry exporter          │
│            │                             │                        │
│            ▼                             ▼                        │
│     gen_synth fixtures          Golden determinism tests          │
└────────────────────────────────────────────────────────────────────┘
```

Gate policy details live in `docs/gates.md`; CI wiring is under
`.github/workflows/verify-bench.yml`.

## Highlights

- Golden digest + explicit tail budgets so regressions fail CI early.
- Observability-first artifacts: `bench.jsonl` + `metrics.prom` for diffing,
  dashboards, and automated SLO checks.
- Conformance + bench scripts are wired for cron / CI, not just local runs.
- CI-ready: determinism, bench, and CodeQL workflows pinned to SHAs.
- Docs Lint is treated as a **required** branch protection check for `main`.
- Designed to slot into HFT / research pipelines as a replay + guardrail
  module rather than a one-off benchmark toy.

## Build

Prereqs: CMake ≥ 3.20, Ninja, modern C++20 compiler, Boost, and
`nlohmann-json`.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ls build/bin/replay
```

macOS (Apple clang hardening):

```sh
cmake --preset macos-appleclang
cmake --build --preset macos-appleclang
```

Notes:

- `build/compile_commands.json` aids IDEs.
- Release builds add stack protector, FORTIFY, PIE when supported.
- Enable sanitizers via `-DENABLE_SANITIZERS=ON` on Debug builds.

## Run

```sh
# Default run
build/bin/replay

# Custom input and limits
build/bin/replay --input path/to/input.bin \
  --gap-ppm 0 --corrupt-ppm 0 --skew-ppm 0 --burst-ms 0
```

Artifacts land in `artifacts/bench.jsonl`, `artifacts/metrics.prom`, and
new HTML analytics dashboard at `artifacts/report/index.html`.
Deterministic fixtures live under `data/golden/`; regenerate with `gen_synth`
as needed.

## Local applications and tools

This repository includes small local applications and tools to help you exercise and validate the engine:

- Applications (see `apps/README.md` for details):
  - `apps/app_main` — minimal config-driven app
  - `apps/test_app` — unit-style test for `app_config`
  - `apps/bench_app` — micro-benchmark for config loading

  Quick build and run (from project root):

  ```sh
  mkdir -p build && cd build
  cmake ..
  make app_main test_app bench_app
  ./apps/app_main
  ./apps/test_app
  ./apps/bench_app
  ```

- Data generation:
  - `build/bin/gen_synth` — create synthetic ITCH-like datasets

  Example:

  ```sh
  ./build/bin/gen_synth --count 200000 --symbols 20 --out data/golden/itch_200k_20sym.bin
  ```

- Offline replay and scaling proof (Phase 6.3–6.5):
  - `build/bin/offline_replay` — partitions by canonical ID % shard count, journals per-shard, and emits a `summary.json` with the canonical aggregate digest.
  - `scripts/validate_digest_consistency.py` — compares aggregate digests across runs (e.g., shards=1 vs shards=4) to prove shard-count invariance.

  Example:

  ```sh
  ./build/bin/offline_replay --input data/golden/itch_200k_20sym.bin --shards 1 --out artifacts/scaling/one
  ./build/bin/offline_replay --input data/golden/itch_200k_20sym.bin --shards 4 --out artifacts/scaling/four
  python3 scripts/validate_digest_consistency.py artifacts/scaling/one/summary.json artifacts/scaling/four/summary.json
  ```

- Demos and examples:
  - `examples/run_demo.py` and `scripts/run_demo.py` — quick entry points to run a small end-to-end demo (see `examples/README.md`).

- Patent evidence packaging (Phase 6.2):
  - `scripts/package_canonical_ids_evidence.sh` — builds/tests and creates a provenance-marked bundle suitable for attorney review.

### Run in Docker

Build image and run containerized replay:

```sh
# Build (from repo root)
docker build -t blanc-quant-lob-engine:local .

# Run default golden replay inside container
docker run --rm blanc-quant-lob-engine:local /app/replay --input /app/data/golden/itch_1m.bin

# Pass a custom file mounted from host
docker run --rm -v "$PWD/data:/data" blanc-quant-lob-engine:local \
  /app/replay --input /data/your_trace.bin
```

## Scripts

```sh
scripts/verify_golden.sh     # digest determinism check
scripts/bench.sh 9           # multi-run benchmark harness
scripts/prom_textfile.sh ... # emit metrics.prom schema
scripts/verify_bench.py      # release gate enforcement
scripts/bench_report.py      # render HTML latency/digest dashboard
```

## Golden-state validation

- Golden digest resides at `data/golden/itch_1m.fnv`.
- `ctest -R golden_state` plus `scripts/verify_golden.sh` ensure
  reproducibility.
- Use `cmake --build build -t golden_sample` (or `make golden`) to refresh
  fixtures after new traces are accepted.

## Developer setup

Ubuntu:

```sh
sudo apt-get update
sudo apt-get install -y cmake ninja-build libboost-all-dev \
  libnlohmann-json3-dev jq
```

macOS:

```sh
brew update
brew install cmake ninja jq nlohmann-json
```

Enable tests with `-DBUILD_TESTING=ON` and run `ctest --output-on-failure -R
book_snapshot` from `build/`. Tests expect `./bin/replay` within working
directory.

## Release packaging

`./scripts/release_package.sh` creates rights-marked zips plus manifests.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./scripts/release_package.sh --build-dir build --art-dir artifacts \
  --out-dir artifacts/release --git-sha "$(git rev-parse --short HEAD)"
```

Add `--sign` for optional detached GPG signatures. `snapshot-nightly`
workflow runs this and uploads bundle automatically.

## Tooling helpers

- `scripts/pin_actions_by_shas.sh` keeps workflow `uses:` entries pinned.
- `.github/workflows/verify-bench.yml` exposes a manual/cron gate run.
- `.github/workflows/determinism.yml` surfaces p50/p95/p99 in job summary and emits notices for easy viewing.
- `.github/workflows/ci.yml` mirrors bench summary surfacing in job summary.
- `.github/workflows/container-scan.yml` pins Trivy to v0.67.2, runs fs & image scans non-blocking, and uploads SARIF to Security tab.
- `docs/technology_transition.md` + `docs/deliverable_marking_checklist.md`
  cover gov delivery and rights-marking guidance.

## CPU pinning (Linux)

```sh
build/bin/replay --input data/golden/itch_1m.bin --cpu-pin 3
# or
CPU_PIN=3 make bench
```

Pinning reduces tail variance on some hosts; measure on your hardware.

**Why core 3 and not core 0?**
Core 0 is the conventional target for OS interrupt affinity on Linux (`/proc/irq/*/smp_affinity`).
The kernel routes timer interrupts, NIC softirqs, and RCU callbacks to core 0 by default on most
distributions. Pinning the replay engine to core 0 would therefore share a physical core with the
OS interrupt handler, injecting unpredictable latency spikes directly into the hot loop.
Core 3 is chosen as the first general-purpose core that is (a) far enough from core 0 to avoid
interrupt coalescing on hyperthreaded die layouts, and (b) not the last core, which some kernels
also use for deferred work. On NUMA machines, verify that core 3 is on the same socket as the
NIC receive queue to avoid cross-socket memory traffic (see `docs/CPU_PINNING_RATIONALE.md`).

**NUMA topology:** Single-socket pinning is the current validated configuration.
Multi-socket NUMA affinity (`numactl`) is documented in `docs/CPU_PINNING_RATIONALE.md`
and `docs/DETERMINISM_ABSOLUTISM.md`; production NUMA validation is planned for the next release.

## Repository layout

```text
include/        # headers
src/            # replay engine, detectors, telemetry
scripts/        # bench, verify, release, pin helpers
artifacts/      # generated outputs (gitignored)
```

## Security & safety

`SECURITY.md` documents coordinated disclosure. CI integrates detect-secrets
and CodeQL. Signing helpers live under `scripts/` if you need to stamp
artifacts. Blanc LOB Engine is opinionated toward safety-by-default: determinism,
repeatable benches, and explicit tail SLOs are non-negotiable controls rather
than after-fact monitoring.

## Contributing

See `CONTRIBUTING.md` for workflow expectations. Pull requests should pin new
dependencies, ship matching tests, and update docs for externally visible
changes.

## License

Distributed under Business Source License 1.1 (`LICENSE`). Research and
non-commercial evaluation are permitted; production use requires a commercial
license until change date defined in `COMMERCIAL_LICENSE.md`.

Research users can clone and run engine today; commercial or production
deployment should follow terms in `COMMERCIAL_LICENSE.md`.

<!-- markdownlint-enable MD013 -->

## Release Information

This release includes prebuilt binaries and necessary artifacts for version 1.00
of Blanc LOB Engine. If you are interested in accessing full source code, please
reach out directly for further details. Project is fully open and available for
students and hobbyists to explore and use.

## Analytics Report Output

This section documents HTML analytics report generated by
`scripts/bench_report.py` and visitor tracking integration.

### Generating Report

Run benchmark report generator after completing benchmark runs:

```sh
python3 scripts/bench_report.py --bench-file artifacts/bench.jsonl \
  --metrics-file artifacts/metrics.prom --output-dir artifacts/report
```

The report will be generated at `artifacts/report/index.html`.

---

## Performance Metrics (Phase 4)

**Last Updated:** 2026-01-25 | **Dataset:** 1M ITCH 5.0 events (32MB)

### Tiered Performance Measurement

Phase 4 introduced `--tier` flag for controlled measurement scope:

| Tier  | Description                         | Performance | Throughput       |
| ----- | ----------------------------------- | ----------- | ---------------- |
| **A** | Match-only (parse + book update)    | **~510ms**  | 1.96M events/sec |
| **B** | In-process end-to-end               | **~813ms**  | 1.23M events/sec |
| **C** | Full proof pipeline (128MB journal) | **~836ms**  | 1.20M events/sec |

**Determinism:** 100% across all tiers - Digest `0x722112ad4c431cb4` verified

### Tier C Stage Breakdown (Representative Run: 668ms)

```text
Parse+Match:      465.5 ms (69.6%)  - ITCH parse + book update
Journal Append:   136.2 ms (20.4%)  - Buffered binary writes (10 syscalls)
Record Populate:   15.1 ms (2.3%)   - Event record prep
Telemetry:          5.2 ms (0.8%)   - JSONL + Prometheus
Other/Unmeasured:  52.3 ms (7.8%)   - OS scheduling, overhead
```

**Max Journal Latency:** 13.7ms (down from 2,518ms in Phase 3!)

### Phase 3 → Phase 4 Improvement

| Metric                  | Phase 3  | Phase 4       | Improvement      |
| ----------------------- | -------- | ------------- | ---------------- |
| **Tier C Total**        | ~60-120s | **~0.5-0.9s** | **~100-200x** ✅ |
| **Journal Overhead**    | 17.4s    | 136ms         | **128x faster**  |
| **Synchronous Flushes** | 100      | 0             | **Eliminated**   |
| **Throughput**          | 2.1 MB/s | 153 MB/s      | **73x increase** |

**Root Cause Fixed:** Removed `file_.flush()` from hot path, increased buffer to 100k records

### Phase 5: Performance Contract (CI-Enforced) ✅

**Objective:** Lock Phase 4 performance as **enforced property** via CI gates

**Capabilities:**

- 🎯 **CI Performance Gates** - 30 iterations per tier, auto-fail on regression
- 📊 **Tier-Specific Budgets** - p50/p95/p99/p999 SLOs with critical thresholds
- 🔬 **Automated Validation** - Determinism + throughput + stability checks
- 📦 **Evidence Bundles** - 365-day retention for compliance/patent filing

**Quick Start:**

```bash
# Run benchmarks (30 iterations per tier)
python3 scripts/run_benchmark.py --tier A --iterations 30 \
  --output artifacts/benchmarks/tier_a.json

# Validate against budgets
python3 scripts/validate_budgets.py \
  --budgets config/perf_budgets.yaml \
  --results artifacts/benchmarks \
  --output artifacts/budget_report.md
```

**CI Integration:** Auto-runs on PR/main push, fails build on regression

**Documentation:**

- [Phase 5 Performance Contract](docs/PHASE5_PERFORMANCE_CONTRACT.md) - CI gates + budgets ✅
- [Phase 4 Complete](docs/PHASE4_COMPLETE.md) - Batch I/O optimization
- [Phase 3 Complete](docs/PHASE3_COMPLETE.md) - Bottleneck identification
- [Phase 2 Results](docs/PHASE2_RESULTS.md) - Binary journal implementation
- [Test Results](docs/TEST_RESULTS_SUMMARY.md) - Validation and testing

### Visitor Badge Integration

The repository uses visitor badges to track page views. Badge format:

**Project badge:**

```markdown
![Visitors](https://visitor-badge.laobi.icu/badge?page_id=jblanc86-maker.blanc-quant-lob-engine)
```

**Issue-specific badge:**

```markdown
![Issue Visitors](https://visitor-badge.laobi.icu/badge?page_id=jblanc86-maker.blanc-quant-lob-engine.issue.<issue_id>)
```

Replace `<issue_id>` with GitHub issue number.
