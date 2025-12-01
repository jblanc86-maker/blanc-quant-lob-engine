#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
set -euo pipefail



runs="${1:-9}"
outfile="artifacts/bench.jsonl"

mkdir -p artifacts
: > "$outfile"

times=()

for i in $(seq 1 "$runs"); do
  line="$(./build/bin/replay --input data/golden/itch_1m.bin)"

  # Extract elapsed_ms from the output line: elapsed_ms=123.45
  ms="$(sed -n 's/.*elapsed_ms=\([0-9.]*\).*/\1/p' <<<"$line")"

  # Fallback if parsing fails for any reason
  ms="${ms:-0}"

  times+=("$ms")

  echo "{\"run\":$i,\"elapsed_ms\":$ms,\"line\":\"$line\"}" >> "$outfile"
done

# If somehow no runs, bail
n=${#times[@]}
if [[ "$n" -eq 0 ]]; then
  echo "no benchmark runs executed"
  exit 1
fi

# Sort times for quantiles
sorted=($(printf "%s\n" "${times[@]}" | sort -n))

# Helper to clamp index into [0, n-1]
idx50=$((n * 50 / 100))
[[ "$idx50" -ge "$n" ]] && idx50=$((n - 1))

idx95=$((n * 95 / 100))
[[ "$idx95" -ge "$n" ]] && idx95=$((n - 1))

idx99=$((n * 99 / 100))
[[ "$idx99" -ge "$n" ]] && idx99=$((n - 1))

p50="${sorted[$idx50]}"
p95="${sorted[$idx95]}"
p99="${sorted[$idx99]}"

echo "p50_ms=$p50 p95_ms=$p95 p99_ms=$p99"

# Append a summary JSON line with quantiles so CI/parsers have easy access
echo "{\"summary\":true,\"runs\":$n,\"p50_ms\":$p50,\"p95_ms\":$p95,\"p99_ms\":$p99}" >> "$outfile"

echo "bench runs complete (runs=$runs)"
