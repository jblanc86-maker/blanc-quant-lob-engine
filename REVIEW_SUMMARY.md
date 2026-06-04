# Engineering Review Summary (Current)

This file tracks the current review posture and what to run locally before pushing.

## Current repository health

- CMake build: passing in local validation.
- CTest suite: passing in local validation.
- Editor diagnostics: no blocking errors.
- Known non-blocking issue: linker warning about duplicate benchmark library in some build configurations.

## Required local run cadence

To reduce regressions, run the following locally before pushing changes:

- **Build + tests (every code change):** build via CMake/Ninja and run CTest.
- **Benchmark gate validation (every performance-sensitive change):** run `scripts/bench.sh` or `scripts/run-benchmark.sh` to produce `artifacts/bench.jsonl` and `artifacts/metrics.prom`, then run `./run_local_checks.sh`.
- **Determinism checks (before release candidates):** re-run golden and digest checks (`scripts/verify_golden.sh`) and compare against expected artifacts.

## Security roadmap alignment

Security work is tracked as a rolling roadmap alongside feature delivery:

- **Now (0-30 days)**
  - Keep vulnerability reporting path current in `SECURITY.md`.
  - Enforce pre-commit hygiene and dependency update review.
  - Ensure release workflows avoid accidental publish paths.
- **Next (30-90 days)**
  - Add lightweight threat-model notes per major subsystem.
  - Add dependency/SBOM automation in CI evidence artifacts.
  - Expand negative/fault-injection tests for parser and replay boundaries.
- **Later (90+ days)**
  - Security hardening checklist for enterprise packaging.
  - Periodic red-team-style replay abuse scenarios and documented mitigations.

## Open follow-ups

- Continue removing stale transitional docs and references where they are no longer part of active scope.
- Keep `README.md`, `ROADMAP.md`, and `SECURITY.md` synchronized when process changes.
