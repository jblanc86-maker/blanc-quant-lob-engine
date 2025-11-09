#!/usr/bin/env bash
set -euo pipefail
out="${1:-artifacts/metrics.prom}"
read p50 p95 p99 < <(scripts/bench.sh 9 | awk '/p50_ms=/{gsub("p50_ms=","");gsub("p95_ms=","");gsub("p99_ms=",""); print $1,$2,$3}')
echo -e "lob_bench_p50_ms $p50\nlob_bench_p95_ms $p95\nlob_bench_p99_ms $p99" > "$out"
echo "wrote $out"
