#!/usr/bin/env bash
# shellcheck shell=bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

add_pythonpath_segment() {
  local segment="$1"
  [[ -d "${segment}" ]] || return 0
  if [[ -z "${PYTHONPATH:-}" ]]; then
    PYTHONPATH="${segment}"
  elif [[ ":${PYTHONPATH}:" != *":${segment}:"* ]]; then
    PYTHONPATH="${segment}:${PYTHONPATH}"
  fi
}

add_pythonpath_segment "${ROOT_DIR}/scripts"
add_pythonpath_segment "${ROOT_DIR}/tools"
add_pythonpath_segment "${ROOT_DIR}/bench"
export PYTHONPATH

printf '[run_local_checks] PYTHONPATH=%s\n' "${PYTHONPATH}"

default_profile="${PROFILE:-standard}"
report_dir="${ROOT_DIR}/artifacts/report-local"

run_default_suite() {
  local bench_file="${ROOT_DIR}/artifacts/bench.jsonl"
  local metrics_file="${ROOT_DIR}/artifacts/metrics.prom"
  if [[ ! -f "${bench_file}" || ! -f "${metrics_file}" ]]; then
    echo "[run_local_checks] expected artifacts missing." >&2
    echo "run scripts/bench.sh or scripts/run-benchmark.sh first to populate artifacts/" >&2
    exit 1
  fi
  echo "[run_local_checks] running verify_bench (profile=${default_profile})"
  python3 "${ROOT_DIR}/scripts/verify_bench.py" --profile "${default_profile}"
  echo "[run_local_checks] rendering bench_report -> ${report_dir}"
  mkdir -p "${report_dir}"
  python3 "${ROOT_DIR}/scripts/bench_report.py" \
    --bench-file "${bench_file}" \
    --metrics-file "${metrics_file}" \
    --output-dir "${report_dir}"
}

if [[ $# -gt 0 ]]; then
  echo "[run_local_checks] executing custom command: $*"
  exec "$@"
else
  run_default_suite
fi
