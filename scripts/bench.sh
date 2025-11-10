#!/usr/bin/env bash
# SPDX-License-Identifier: BUSL-1.1
set -euo pipefail
runs="${1:-9}"
outfile="artifacts/bench.jsonl"
mkdir -p artifacts
times=()
for i in $(seq 1 "$runs"); do
  line="$(./build/replay --input data/golden/itch_1m.bin)"
  ms="$(sed -n 's/.*elapsed_ms=\([0-9.]*\).*/\1/p' <<<"$line")"
  times+=("$ms")
  echo "{\"run\":$i,\"elapsed_ms\":$ms,\"line\":\"$line\"}" >> "$outfile"
done
# quick quantiles
sorted=($(printf "%s\n" "${times[@]}" | sort -n))
n=${#sorted[@]}
p50=${sorted[$((n*50/100))]}
p95=${sorted[$((n*95/100))]}
p99=${sorted[$((n*99/100<n? n-1 : n-1))]}
echo "p50_ms=$p50 p95_ms=$p95 p99_ms=$p99"
