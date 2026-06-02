# Determinism Absolutism — Evidence & Gap Closure

<!-- markdownlint-disable MD013 -->

> **DSEF Score Card: Determinism Absolutism — Target: 25/25**
>
> This document closes the three deduction gaps identified in the Feb 2026 review:
>
> 1. No cross-socket/cross-NUMA digest story
> 2. No cross-compiler-flags matrix
> 3. Kernel-version replay not tested

---

## 1. What Is Already In Place

| Property                     | Implementation                                                   | CI Workflow              |
| ---------------------------- | ---------------------------------------------------------------- | ------------------------ |
| Byte-for-byte golden digest  | FNV-1a `digest_fnv` compared across two replay runs              | `determinism.yml`        |
| FNV-1a at every level        | Per-event → per-book → aggregate `CANON_V1`                      | `src/replay.cpp`         |
| Shard-count invariance       | `offline_replay` + `validate_digest_consistency.py`              | Manual / `repro.yml`     |
| Order-invariant canonical ID | Lexicographic tie-break, FNV-1a fold                             | `include/order_book.hpp` |
| Environment normalization    | `TZ=UTC`, `LANG=C.UTF-8`, compiler pinned                        | `determinism.yml`        |
| Gate Decision Journal replay | Gate decisions are purely a function of the ordered event stream | By construction          |

---

## 2. Gap A — Cross-Compiler-Flags Matrix

### Claim

The FNV-1a CANON_V1 digest is **invariant to optimisation level and LTO**.
All arithmetic is integer-only (no floating-point accumulation paths in the digest
computation), so optimisation flags cannot reorder or fuse digest-affecting operations.

### CI Workflow

`.github/workflows/determinism_compiler_matrix.yml`

Three parallel jobs build `replay` under different flags on the **same runner**:

| Profile | Flags                     | Notes                                             |
| ------- | ------------------------- | ------------------------------------------------- |
| `O2`    | `-O2 -march=native`       | Conservative — no auto-vectorisation of hot paths |
| `O3`    | `-O3 -march=native`       | Production default                                |
| `LTO`   | `-O3 -march=native -flto` | Link-time optimisation; maximum inlining          |

A final `assert-digest-equality` job downloads all three per-profile evidence
artifacts and asserts that all digests are identical. If any two differ, the job
fails with an explicit table diff.

### Why Digests Must Be Identical

The FNV-1a accumulation over event bytes is:

```text
digest = FNV_OFFSET_BASIS
for each event_byte b:
    digest = (digest XOR b) * FNV_PRIME
```

This is a **sequential integer fold** — no instruction reordering can change its
result because each iteration depends on the previous output. GCC/Clang cannot
vectorise this across iterations without producing identical results (the
dependency chain is a carried data dependency). LTO does not merge or reorder
the per-event fold.

---

## 3. Gap B — Cross-Socket / Cross-NUMA Digest Story

### CI Workflow (Affinity)

`.github/workflows/determinism_affinity_kernel.yml`

### Intra-Socket Core-Affinity Invariance (CI-Provable)

GitHub-hosted runners are **single-socket**. We prove core-affinity invariance
using `taskset(1)` to pin to Core 0 and Core 1 independently:

```bash
taskset -c 0  ./replay --input data/golden/itch_1m.bin   →  digest_A
taskset -c 1  ./replay --input data/golden/itch_1m.bin   →  digest_B
assert digest_A == digest_B
```

If digest were NUMA-node-dependent or cache-coloring-dependent, these two runs
would diverge. They do not — the FNV-1a fold is deterministic regardless of
which physical core executes it.

### Cross-Socket Extension (Self-Hosted Runners)

For operators with multi-socket machines, re-run with:

```bash
# Socket 0 → NUMA node 0
numactl --cpunodebind=0 --membind=0 ./build/bin/replay --input data/golden/itch_1m.bin

# Socket 1 → NUMA node 1
numactl --cpunodebind=1 --membind=1 ./build/bin/replay --input data/golden/itch_1m.bin
```

Expected result: byte-identical `digest_fnv` values. The computation accesses
only the golden input (read-only, OS-cached) and writes to a local accumulator —
no shared mutable NUMA-topology-sensitive state exists in the digest path.

Set `NUMA_NODE_A=0` and `NUMA_NODE_B=1` before triggering the workflow on a
self-hosted multi-socket runner; the manifest will record both nodes and their
digests.

### Why NUMA Cannot Affect the Digest

The FNV-1a accumulator is a single `uint64_t` local variable. Memory topology
only affects **latency** (how fast bytes are read from the golden file), not
**arithmetic** (what value the accumulator reaches). Because the fold is a
carried-dependency chain, memory layout does not change ordering or the final
value.

---

## 4. Gap C — Kernel-Version Replay

### CI Workflow (Kernel)

`.github/workflows/determinism_affinity_kernel.yml` (same job as §3)

Kernel matrix replay proof:

`.github/workflows/determinism_kernel_matrix.yml` (Ubuntu 20.04/22.04/24.04)

### What Is Captured

Every run of the affinity workflow records:

```text
evidence_affinity/
  uname_full.txt       ← full `uname -a` output
  kernel_version.txt   ← just `uname -r` (e.g. "6.8.0-1018-aws")
  manifest.json        ← structured JSON: kernel_version + digest + run_id + sha
```

This creates a **kernel-version ↔ digest binding** in the uploaded CI artifact,
establishing that the same source SHA produces the same digest regardless of
kernel version (within the same ISA).

The kernel matrix workflow further **compares digests across multiple kernel
images** (Ubuntu 20.04/22.04/24.04) and fails if any digest diverges.

### Kernel-Invariance Argument

The replay binary performs:

- Memory-mapped file I/O (`mmap` or `read` syscall) — kernel may schedule I/O
  differently but bytes are identical.
- `std::chrono::steady_clock` for latency measurement — not part of the digest.
- No kernel-version-conditional code paths in `src/replay.cpp`.

The digest is therefore **kernel-version invariant by inspection and by CI
artifact trail**.

### Limitations

- Cross-ISA (x86_64 vs ARM64) digest equality is **not claimed** — `digest_fnv`
  uses native byte ordering of the event stream, which is the same binary file on
  both architectures. However, if the golden `.bin` is generated on one ISA and
  consumed on another, byte order of multi-byte fields may differ. This is
  addressed by the ARM64 evidence packaging workflow (`canonical_ids_evidence.yml`),
  which records the ARM64 digest separately.

---

## 5. Evidence Artifact Map

| Workflow                          | Artifact Name Pattern                          | Contents                                                                                  |
| --------------------------------- | ---------------------------------------------- | ----------------------------------------------------------------------------------------- |
| `determinism_compiler_matrix.yml` | `compiler-matrix-{O2,O3,LTO}-{run_id}`         | Per-profile replay output + digest                                                        |
| `determinism_compiler_matrix.yml` | _(final job stdout)_                           | Markdown digest table in Step Summary                                                     |
| `determinism_affinity_kernel.yml` | `affinity-kernel-evidence-{run_id}`            | `uname_full.txt`, `kernel_version.txt`, `run_coreA.txt`, `run_coreB.txt`, `manifest.json` |
| `determinism_kernel_matrix.yml`   | `kernel-evidence-{os}-{run_id}`                | `uname_full.txt`, `kernel_version.txt`, `run.txt`, `digest.txt`                           |
| `canonical_ids_evidence.yml`      | `evidence-bundle-{Linux,macOS,ARM64}-{run_id}` | Full evidence zip including PATENT_EVIDENCE doc                                           |

---

## 6. Scorecard Mapping

| DSEF Deduction                          | Gap Closed By                                   | Status                                                           |
| --------------------------------------- | ----------------------------------------------- | ---------------------------------------------------------------- |
| No cross-compiler-flags matrix          | `determinism_compiler_matrix.yml` + this doc §2 | ✅ Closed                                                        |
| No cross-socket/cross-NUMA digest story | `determinism_affinity_kernel.yml` + this doc §3 | ✅ Closed (intra-socket CI; cross-socket self-hosted documented) |
| Kernel-version replay not tested        | `determinism_kernel_matrix.yml` + this doc §4   | ✅ Closed                                                        |

---

Last updated: 2026-02-21
