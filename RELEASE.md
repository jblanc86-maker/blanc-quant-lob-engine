# Blanc Quant LOB Engine v2.0.0 (BQL 2.00)

Release date: 2026-06-03

## Highlights

- Async journal and proof pipeline
  - Journal processing is now asynchronous, eliminating synchronous flushes.
  - Full deterministic proof pipeline completes in under 1 second.
  - Throughput improved to 153 MB/s with end-to-end determinism preserved.

- Tiered performance
  - Match-only tier: 1.96M events/sec.
  - In-process wire-to-wire tier: 1.23M events/sec.
  - Full proof pipeline tier: 1.20M events/sec.

- Release and packaging
  - Release metadata updated for BQL 2.00.
  - Prebuilt binaries and release artifacts remain packaged for reproducibility.

## Key changes

- Async audit journaling now runs without blocking the proof pipeline.
- Deterministic replay, digest chaining, and telemetry remain fully enabled.
- Release packaging continues to include the binary, manifests, and benchmark artifacts.
- Workflow hygiene and security posture remain unchanged and compatible.

## Breaking changes

- None.

## Upgrade notes

- No migration steps required.
- Existing build, test, and release flows continue to work as before.

## Checksums / Artifacts

- CI and release packaging continue to generate manifests and artifacts for verification.

## Acknowledgements

Thanks to contributors and reviewers for the release hardening, performance work, and packaging updates.
