#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
set -euo pipefail

echo "Verifying binaries..."
file build/bin/* || true

if [[ -x build/bin/gen_synth && -x build/bin/replay ]]; then
  echo "Running quick smoke test..."
  ./build/bin/gen_synth
  ./build/bin/replay --help
fi
