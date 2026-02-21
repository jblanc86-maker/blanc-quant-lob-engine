#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
# Long-horizon stability run (tail latency + gate behavior drift).
#
# Defaults to 24h on self-hosted runners. Configure with:
#   DURATION_HOURS=24 INTERVAL_SEC=300 REPLAY_BIN=build/bin/replay ./scripts/long_horizon_stability.sh
set -euo pipefail

DURATION_HOURS=${DURATION_HOURS:-24}
INTERVAL_SEC=${INTERVAL_SEC:-300}
REPLAY_BIN=${REPLAY_BIN:-build/bin/replay}
INPUT_BIN=${INPUT_BIN:-data/golden/itch_1m.bin}
OUT_DIR=${OUT_DIR:-artifacts/stability}

mkdir -p "${OUT_DIR}"

start_ts=$(date -u +%s)
end_ts=$(( start_ts + DURATION_HOURS * 3600 ))
iter=0

summary_file="${OUT_DIR}/stability_summary.jsonl"

log() {
  echo "[$(date -u +%Y-%m-%dT%H:%M:%SZ)] $*" >&2
}

extract_metrics() {
  local json_line=$1
  if command -v jq >/dev/null 2>&1; then
    jq -r '{p50_ms,p95_ms,p99_ms,p999_ms,p9999_ms,digest_fnv,determinism,cpu_pin}' <<<"$json_line" 2>/dev/null || true
  else
    echo "{}"
  fi
}

while (( $(date -u +%s) < end_ts )); do
  iter=$((iter + 1))
  run_dir="${OUT_DIR}/run_${iter}"
  mkdir -p "$run_dir"

  log "Run ${iter}: starting replay"
  "${REPLAY_BIN}" --input "${INPUT_BIN}" | tee "${run_dir}/replay_stdout.txt"

  # Copy latest artifacts if present
  if [ -f artifacts/bench.jsonl ]; then
    cp artifacts/bench.jsonl "${run_dir}/bench.jsonl"
    last_line=$(tail -n1 artifacts/bench.jsonl || true)
  else
    last_line="{}"
  fi

  if [ -f artifacts/metrics.prom ]; then
    cp artifacts/metrics.prom "${run_dir}/metrics.prom"
  fi

  if [ -f artifacts/report/index.html ]; then
    cp artifacts/report/index.html "${run_dir}/report_index.html"
  fi

  metrics=$(extract_metrics "$last_line")
  cat >> "${summary_file}" <<EOF
{"timestamp":"$(date -u +%Y-%m-%dT%H:%M:%SZ)","iteration":${iter},"metrics":${metrics}}
EOF

  log "Run ${iter}: captured metrics"
  sleep "${INTERVAL_SEC}"

done

log "Long-horizon run complete: ${iter} iterations over ${DURATION_HOURS}h"
