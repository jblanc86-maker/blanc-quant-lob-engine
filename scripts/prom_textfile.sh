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
p50=${p50:-0}
p95=${p95:-0}
p99=${p99:-0}
tmp_out=$(mktemp /tmp/lob_metrics.XXXXXX.prom)

if [[ -f "$out" ]]; then
  awk '!/^lob_bench_/ && $1 != "lob_p50_ms" && $1 != "lob_p95_ms" && $1 != "lob_p99_ms"' "$out" > "$tmp_out"
else
  : > "$tmp_out"
fi

ensure_metric() {
  local key="$1"
  local value="$2"
  if ! grep -q "^${key} " "$tmp_out"; then
    printf '%s %s\n' "$key" "$value" >> "$tmp_out"
  fi
}

existing_publish=$(awk '/^lob_publish_allowed / {print $2; exit}' "$out" 2>/dev/null || true)
if [[ -z "${existing_publish:-}" ]]; then
  existing_publish=1
fi

ensure_metric "lob_publish_allowed" "$existing_publish"

{
  printf 'lob_p50_ms %s\n' "$p50"
  printf 'lob_p95_ms %s\n' "$p95"
  printf 'lob_p99_ms %s\n' "$p99"
  printf 'lob_bench_p50_ms %s\n' "$p50"
  printf 'lob_bench_p95_ms %s\n' "$p95"
  printf 'lob_bench_p99_ms %s\n' "$p99"
} >> "$tmp_out"

mv "$tmp_out" "$out"
echo "wrote $out"
