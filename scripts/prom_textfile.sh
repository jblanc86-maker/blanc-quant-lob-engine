#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
set -euo pipefail
out="${1:-artifacts/metrics.prom}"
ART_DIR="${ART_DIR:-artifacts}"
CPU_PIN_ARG=""
if [[ -n "${CPU_PIN:-}" ]]; then
  CPU_PIN_ARG="--cpu-pin ${CPU_PIN}"
fi
# Run bench in the same ART_DIR and with CPU_PIN if provided, then parse the quantiles.
read p50 p95 p99 < <(ART_DIR="$ART_DIR" CPU_PIN="${CPU_PIN:-}" scripts/bench.sh 9 | awk '/p50_ms=/{gsub("p50_ms=","");gsub("p95_ms=","");gsub("p99_ms=",""); print $1,$2,$3}')
tmp_out=$(mktemp /tmp/lob_metrics.XXXXXX.prom)
echo -e "lob_bench_p50_ms $p50\nlob_bench_p95_ms $p95\nlob_bench_p99_ms $p99" > "$tmp_out"
# If replay wrote a telemetry metrics.prom into ART_DIR, append lob_cpu_pin from it (best-effort).
if [[ -f "$ART_DIR/metrics.prom" ]]; then
  lob_cpu_pin_value=$(grep -E '^lob_cpu_pin ' "$ART_DIR/metrics.prom" | awk '{print $2}' || true)
  if [[ -n "$lob_cpu_pin_value" ]]; then
    echo "lob_cpu_pin $lob_cpu_pin_value" >> "$tmp_out"
  fi
fi
mv "$tmp_out" "$out"
echo "wrote $out"
