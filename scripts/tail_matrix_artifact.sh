#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
# Generate tail-latency truthfulness artifacts for sample-size matrix.
#
# Matrix:
#   100x 100k samples  (count=800000)
#    10x 250k samples  (count=2000000)
#    10x 1M samples    (count=8000000)
#
# Output:
#   artifacts/tail_matrix.jsonl
#
# Each line includes: timestamp, count, samples, elapsed_ms, digest_fnv, p99, p99.9, p99.99, validity flags.
set -euo pipefail

REPLAY_BIN=${REPLAY_BIN:-build/bin/replay}
OUT_DIR=${OUT_DIR:-artifacts}
OUT_FILE="${OUT_DIR}/tail_matrix.jsonl"

mkdir -p "${OUT_DIR}"

run_batch() {
  local count=$1
  local iterations=$2
  local input="data/golden/itch_${count}.bin"

  if [ ! -f "$input" ]; then
    ./build/bin/gen_synth --count "$count" --out "$input"
  fi

  for ((i=1; i<=iterations; i++)); do
    local line
    line=$($REPLAY_BIN --input "$input" | tr '\n' ' ')
  # Extract metrics from stdout (use Python regex for portability)
  local elapsed samples digest p99 p999 p9999 v999 v9999
  parsed=$(LINE="$line" python3 - <<'PY'
import os, re
line = os.environ.get("LINE", "")
def grab(pattern):
  m = re.search(pattern, line)
  return m.group(1) if m else ""
fields = [
  grab(r"elapsed_ms=([0-9.]+)"),
  grab(r"samples=([0-9]+)"),
  grab(r"digest_fnv=(0x[0-9a-f]+)"),
  grab(r"\sp99=([0-9.eE+-]+)ms"),
  grab(r"\sp99\.9=([0-9.eE+-]+)ms"),
  grab(r"\sp99\.99=([0-9.eE+-]+)ms"),
  grab(r"p99\.9_valid=(true|false)"),
  grab(r"p99\.99_valid=(true|false)"),
]
print(" ".join(fields))
PY
)
  read -r elapsed samples digest p99 p999 p9999 v999 v9999 <<<"$parsed"

    cat >> "$OUT_FILE" <<EOF
{"ts":"$(date -u +%Y-%m-%dT%H:%M:%SZ)","count":${count},"samples":${samples:-0},"elapsed_ms":${elapsed:-0},"digest_fnv":"${digest:-unknown}","p99_ms":${p99:-0},"p999_ms":${p999:-0},"p9999_ms":${p9999:-0},"p999_valid":${v999:-false},"p9999_valid":${v9999:-false}}
EOF
  done
}

# Reset output
: > "$OUT_FILE"

# Run matrix
run_batch 800000 100
run_batch 2000000 10
run_batch 8000000 10

echo "Wrote $OUT_FILE"
