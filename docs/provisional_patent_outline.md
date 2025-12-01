# Provisional Patent Specification Outline

This document is a draft provisional patent outline for the core inventions embodied in the Blanc LOB Engine. It is intended to be used as a technical resource for patent counsel and engineering teams when preparing a provisional patent filing and figure set.

Summary of innovations
-----------------------

- Deterministic replay of time-ordered event streams (e.g., ITCH) with canonical serialization and auditable end-state digests (e.g., FNV rolling hash) for reproducible verification.
- Breaker-style gating logic integrated into a low-latency signal processing pipeline that mitigates pathological conditions while preserving tail-latency bounds.
- Telemetry-based golden-state validation and CI integration for deterministic regression testing and artifact packaging with rights-marked deliverables.

## Title

Deterministic Market Data Replay Engine with Breaker-style Gating and Telemetry-based Golden-State Validation

## Technical Field

Systems and methods for deterministic replay of event streams, order-book maintenance, anomaly detectors, gating of processing paths, and telemetry-driven golden-state validation.

## Background

Conventional replays and benchmarking are often non-deterministic due to OS scheduling, concurrent updates, and floating point inconsistencies. Breaker or gating logic historically trades off performance for safety and may unknowingly widen tail latency. That creates a need for a combined approach with reproducible replay, low-latency gating, and CI-auditable telemetry.

## Brief Summary

The invention encompasses a deterministic replay engine that reconstructs order-book state from a captured time-ordered log, computes a canonical digest for end-state verification, and integrates detector-driven breaker logic that reduces exposure to pathological event sequences without widening tail latency. The system emits structured telemetry and produces rights-marked artifacts for reproducibility and audit.

## Brief Description of the Figures (suggested)

- FIG. 1: System architecture — capture, replay engine, order-book core, detectors, breaker gates, telemetry, bench/CI.
- FIG. 2: Event replay dataflow — event ingestion, deterministic timestamp mapping, book update, detector input.
- FIG. 3: Order-book structure (SoA) and canonical serialization order.
- FIG. 4: Breaker finite-state machine diagram (NORMAL, THROTTLE, ISOLATE, RECOVER, inputs & timers).
- FIG. 5: Deterministic digest pseudocode & canonicalization rules.
- FIG. 6: Telemetry JSONL schema & Prom mapping (p50/p95/p99, cpu_pin, detectors, publish flag).
- FIG. 7: Golden-state CI flow — bench.jsonl → golden comparison → upload artifacts.
- FIG. 8: Packaging & rights manifest flow — ART_DIR → artifact zip → signature → rights manifest.
- FIG. 9: Example latency distributions (baseline vs gated) showing p50/p95/p99.

## Detailed Description of Example Embodiments

### Deterministic replay engine

The deterministic replay engine reads a time-ordered event capture and applies events to an in-memory order-book. Key features to be specified:

- Deterministic ordering: strict, canonical read order of events from the capture; deterministic ordering for concurrent operations and iteration patterns.
- Deterministic timing mapping: deterministic clock mapping rules so that timing variance does not affect canonical final state (burst emulation parameterized by --burst-ms can be recorded as telemetry).
- CPU pinning option to reduce system scheduling noise (record cpu_pin as telemetry when used).

### Canonical digest generation

- Define canonical serialization of order-book state (price ascending, per-side canonical order); compute a rolling hash (e.g., FNV) with a defined endianness and canonical field ordering to produce a reproducible digest.

### Detectors and metrics

- Windowed metrics (gap_ppm, corrupt_ppm, skew_ppm, burst_ms) and detector thresholding components produce anomaly scores for use by the gating logic.

### Breaker-style gating logic

- Breaker is implemented as a deterministic finite-state machine with states: NORMAL, THROTTLE, ISOLATE, RECOVER.
- Transition conditions are defined by time-windowed detector metrics and optional hysteresis/timers to avoid flip-flops.
- The breaker alters data-flow or resource allocation only in ways that preserve bounded tail-latency (e.g., priority scheduling, skimming lower-priority paths) rather than introducing long stalls.

*** End Patch
- Structured JSONL telemetry (bench.jsonl) with standard fields: actual, golden, determinism flag, p50/p95/p99, cpu_pin, gap_ppm, corrupt_ppm, skew_ppm, burst_ms, breaker, publish flag.
- CI integration for golden comparison; handling of mismatches (fail vs collect artifacts).

### Deliverable packaging and provenance

- ART_DIR artifacts, rights manifest, signed package, and metadata manifest (including git SHA, build environment).

## Example Implementations and Use Cases
- Baseline and burst-mode replay; how gating is evaluated; p50/p95/p99 comparison and pass/fail criteria.
- Determinism tests and regeneration workflows (how to regenerate golden if allowed by policy).

## Advantages and Effects
- Deterministic verification for regression testing and reproducible benchmarking.
- Breaker logic prevents tail-widening and offers fault-isolation with predictable performance guarantees.

## Example (Draft) Claims
Draft claims should be refined with counsel. Examples:

System Claim (draft):
1. A computer-implemented system comprising: a deterministic replay component configured to apply time-ordered events to an in-memory order-book model; a digest module configured to generate a canonical digest of the order-book state; a detector configured to compute metrics over sliding windows; and a gating component configured to alter processing behavior based on detector inputs to mitigate pathological conditions while preserving tail-latency constraints.

Method Claim (draft):
2. A method comprising: reading a time-ordered event capture; applying events deterministically to an order-book structure; computing a canonical digest; monitoring detectors; and invoking gating decisions to avoid pathological conditions while maintaining tail-latency bounds.

Dependent Claim Examples (draft):
3. The system of claim 1, wherein the detector computes gap_ppm, corrupt_ppm, and skew_ppm metrics.
4. The system of claim 1, wherein the digest is a rolling FNV-style hash computed over a canonical serialization.

## Drawings and Figure Captions (for counsel to convert into figure files)
- FIG. 1: System architecture diagram with component boxes and arrows.
- FIG. 2: Replay dataflow showing event ingestion, deterministic scheduler, book update, detectors, gate insertion.
- FIG. 3: SoA memory layout and canonical serialization order explanation.
- FIG. 4: Breaker FSM with labeled transitions and hysteresis timers.
- FIG. 5: Digest algorithm pseudocode and canonical serialization example.
- FIG. 6: Telemetry JSON schema and sample `bench.jsonl` entry.
- FIG. 7: CI golden-state validation & artifact lifecycle.
- FIG. 8: Packaging rights manifest and signed artifact flow diagram.
- FIG. 9: Example histograms / violin plots for latency analysis.

11. Next Steps & Deliverables for Counsel
---------------------------------------
1. Add simplified pseudocode for the deterministic loop and the breaker's state machine (sufficiently detailed but not production code).
2. Generate FIG. 1–9 with vector diagrams (SVG), and annotate the diagrams with parameter/threshold names used in the detectors and gates.
3. Draft final provisional-language claims with counsel and include the figure set in the provisional filing.

Note: keep the system modularly partitioned so commercialization options can be selected (e.g., keep gating logic proprietary and open-source the telemetry export if desired).

-- End of provisional patent outline --
