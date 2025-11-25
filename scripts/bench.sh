#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
set -euo pipefail

runs="${1:-9}"
outfile="artifacts/bench.jsonl"

mkdir -p artifacts
# start fresh each bench run
: > "$outfile"

for i in $(seq 1 "$runs"); do
  line="$(./build/bin/replay --input data/golden/itch_1m.bin)"
  # replay doesn't emit elapsed_ms yet, so just log 0 for now
  ms=0
  echo "{\"run\":$i,\"elapsed_ms\":$ms,\"line\":\"$line\"}" >> "$outfile"
done

echo "bench runs complete (runs=$runs)"
