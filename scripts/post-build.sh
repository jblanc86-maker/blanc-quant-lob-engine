#!/usr/bin/env bash
set -euo pipefail

echo "Verifying binaries..."
file build/bin/* || true

if [[ -x build/bin/gen_synth && -x build/bin/replay ]]; then
  echo "Running quick smoke test..."
  ./build/bin/gen_synth
  ./build/bin/replay --help
fi
