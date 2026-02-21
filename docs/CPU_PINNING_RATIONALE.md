# CPU Pinning Rationale

<!-- markdownlint-disable MD013 -->

> **DSEF Hubris Calibration — gap closure:**
> "Core 0 avoidance rationale is not explained" and
> "No dedicated 'Why core 3 and not core 0?' note in README or docs."

---

## TL;DR

Use `--cpu-pin 3` (or any core ≥ 2 on your specific machine).
**Do not use core 0.**
Verify your choice with `perf stat -e cs` and a tail-latency run before committing to production.

---

## Why Not Core 0?

On Linux, core 0 is the default home for a wide class of kernel activity:

| Source | Mechanism | Effect on core 0 |
| --- | --- | --- |
| Hardware interrupts | `/proc/irq/*/smp_affinity` defaults to CPU mask `0x1` (core 0) | NIC, disk, timer IRQs run on core 0 |
| Soft IRQ / NAPI | `ksoftirqd/0` is bound to core 0 | Network packet processing competes with user threads |
| RCU callbacks | `rcu_sched` prefers core 0 on many kernels | Memory reclaim work injected into core 0's run queue |
| Timer wheel | `hrtimer` softirq lands on core 0 unless `nohz_full=` is set | Sub-ms jitter at every tick boundary |
| `kthreadd` children | Kernel threads spawned from PID 2 inherit core 0 affinity | Any new driver or workqueue fires there first |

A replay run on core 0 will share the physical core with all of the above unless you have
explicitly re-pinned every interrupt (`/proc/irq/*/smp_affinity`) and set `nohz_full=1-N`
in the kernel command line. That is a valid HFT production setup, but it requires root and
a kernel reboot — it is not safe to assume as a default.

**Core 0 is therefore the worst default choice for latency-sensitive work without a fully
isolated kernel configuration.**

---

## Why Core 3?

Core 3 satisfies several practical criteria without requiring kernel configuration changes:

1. **Not core 0** — avoids the interrupt-affinity default.
2. **Not core 1** — on hyperthreaded CPUs, core 1 is often the sibling thread of core 0
   (logical CPUs 0 and 1 share the same physical core on Intel SMT). Running on core 1 gives
   you physical core isolation from user code but *not* from hardware interrupts, because SMT
   siblings share the branch predictor, L1, and L2 with core 0.
3. **Not the last core** — some kernels and BIOS implementations use the highest-numbered
   core for deferred work queues (`workqueue/N`) or watchdog threads (`watchdog/N`).
4. **Low enough index to be on NUMA node 0** — on a typical 2-socket server with 8 cores
   per socket, cores 0–7 are on socket 0. Core 3 is comfortably within socket 0.
5. **Documented and reproducible** — using a named constant (`--cpu-pin 3`) rather than
   a dynamic "pick the quietest core" heuristic makes the telemetry record (`cpu_pin: 3`)
   stable across runs and machines of the same class.

### Choosing a Different Core

If your server has a different topology, run:

```bash
# Show interrupt affinity
cat /proc/irq/*/smp_affinity_list 2>/dev/null | sort -u

# Show SMT siblings
cat /sys/devices/system/cpu/cpu*/topology/thread_siblings_list | sort -u

# Show NUMA topology
numactl --hardware
```

Pick a core that is not in the interrupt affinity set and not the SMT sibling of core 0.
Common production choices: 3, 4, 5 (socket 0, post-sibling, pre-last).

---

## NUMA Topology: Current State

| Configuration | Status |
| --- | --- |
| Single-socket, core 3 | ✅ Validated — documented default |
| Single-socket, any non-0 core | ✅ Supported — use `--cpu-pin N` |
| Multi-socket NUMA (numactl) | 📋 Documented (see below), not yet CI-validated |
| Cross-NUMA digest invariance | 📋 Documented in `docs/DETERMINISM_ABSOLUTISM.md` §3 |

Multi-socket NUMA affinity is the *next* validated configuration.
The FNV-1a digest is NUMA-invariant by construction (no NUMA-topology-sensitive
state in the accumulator path), but systematic latency characterization across
NUMA nodes has not been run at production scale. See `ROADMAP.md` for the
tracking item.

### Multi-Socket NUMA Setup (Preview)

For operators who want to evaluate now:

```bash
# Pin to socket 0, core 3 — local memory
numactl --cpunodebind=0 --membind=0 ./build/bin/replay \
  --input data/golden/itch_1m.bin --cpu-pin 3

# Pin to socket 1, core 11 (first non-0 core on node 1)
numactl --cpunodebind=1 --membind=1 ./build/bin/replay \
  --input data/golden/itch_1m.bin --cpu-pin 11
```

Both runs should produce the same `digest_fnv` value and similar (but not identical)
latency profiles. Cross-socket memory latency will increase p99 — that is expected and
is not a determinism failure.

---

## Telemetry Record

Every run emits `cpu_pin` in both the JSONL and Prometheus outputs:

```json
{"cpu_pin": 3, "p50_ms": 0.012, "p99_ms": 0.045, ...}
```

```text
lob_cpu_pin 3
```

This makes the pinning configuration **part of the verifiable evidence record** for every
benchmark run. A run with `cpu_pin: 0` in the artifact can be flagged as potentially
noise-contaminated during review.

---

## Release Notes Reference

CPU pinning was introduced in the initial release as a latency-stabilization feature.
NUMA topology support is tracked in `ROADMAP.md` as a Phase 4 item.

---

Last updated: 2026-02-21
