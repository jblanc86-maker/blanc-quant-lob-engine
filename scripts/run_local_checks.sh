#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

augment_pythonpath() {
  local candidate
  local additions=()
  for candidate in "$@"; do
    if [[ -d "$candidate" ]]; then
      case ":${PYTHONPATH:-}:" in
        *:"$candidate":*) ;;
        *) additions+=("$candidate") ;;
      esac
    fi
  done
  if ((${#additions[@]} > 0)); then
    local joined
    IFS=:
    joined="${additions[*]}"
    unset IFS
    if [[ -n "${PYTHONPATH:-}" ]]; then
      export PYTHONPATH="${joined}:${PYTHONPATH}"
    else
      export PYTHONPATH="$joined"
    fi
  fi
}

augment_pythonpath "$REPO_ROOT" "$REPO_ROOT/scripts"

RUN_PRE_COMMIT=true
RUN_CONFIGURE=true
RUN_BUILD=true
RUN_TESTS=true
RUN_BENCH=false
PRINT_ENV=false
CUSTOM_COMMAND=()

usage() {
  cat <<'USAGE'
Usage: scripts/run_local_checks.sh [options] [-- command]

Options:
  --skip-pre-commit   Skip pre-commit hooks
  --skip-configure    Skip CMake configure step
  --skip-build        Skip the build step
  --skip-tests        Skip ctest execution
  --bench             Run bench + verify_bench gate
  --print-env         Print effective PYTHONPATH and exit unless -- command follows
  --help              Show this help text

If "--" is provided, remaining arguments are executed with the configured PYTHONPATH.
USAGE
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --skip-pre-commit) RUN_PRE_COMMIT=false ;;
    --skip-configure) RUN_CONFIGURE=false ;;
    --skip-build) RUN_BUILD=false ;;
    --skip-tests) RUN_TESTS=false ;;
    --bench) RUN_BENCH=true ;;
    --print-env) PRINT_ENV=true ;;
    --help|-h) usage; exit 0 ;;
    --)
      shift
      CUSTOM_COMMAND=("$@")
      break
      ;;
    *)
      echo "Unknown option: $1" >&2
      usage >&2
      exit 64
      ;;
  esac
  shift
done

if $PRINT_ENV; then
  echo "PYTHONPATH=${PYTHONPATH:-}"
  if ((${#CUSTOM_COMMAND[@]} == 0)) && ! $RUN_PRE_COMMIT && ! $RUN_CONFIGURE && ! $RUN_BUILD && ! $RUN_TESTS && ! $RUN_BENCH; then
    exit 0
  fi
fi

if ((${#CUSTOM_COMMAND[@]} > 0)); then
  exec "${CUSTOM_COMMAND[@]}"
fi

if $RUN_PRE_COMMIT; then
  if command -v pre-commit >/dev/null 2>&1; then
    pre-commit run -a
  else
    echo "pre-commit not found; skipping lint hooks." >&2
  fi
fi

if $RUN_CONFIGURE; then
  cmake -S "$REPO_ROOT" -B "$REPO_ROOT/build" -G Ninja -DCMAKE_BUILD_TYPE=Release
fi

if $RUN_BUILD; then
  cmake --build "$REPO_ROOT/build" -j
fi

if $RUN_TESTS; then
  if [[ -d "$REPO_ROOT/build" ]]; then
    ctest --output-on-failure --test-dir "$REPO_ROOT/build"
  else
    echo "build directory missing; skipping ctest." >&2
  fi
fi

if $RUN_BENCH; then
  local_art_dir="$REPO_ROOT/artifacts/local-checks"
  make -C "$REPO_ROOT" bench ART_DIR="$local_art_dir"
  python3 "$REPO_ROOT/scripts/verify_bench.py" \
    --bench-file "$local_art_dir/bench.jsonl" \
    --metrics-file "$local_art_dir/metrics.prom" \
    --baseline-file "$REPO_ROOT/artifacts/baselines/sample_runner.json" \
    --profile "${PROFILE:-standard}" \
    --runner-id "${RUNNER_ID:-manual-runner}"
fi
