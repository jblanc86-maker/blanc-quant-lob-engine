#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
set -euo pipefail

INPUT="data/golden/itch_1m.bin"
GENERATOR="./build/gen_synth"

if [ ! -x "$GENERATOR" ]; then
  echo "expected $GENERATOR to exist" >&2
  exit 1
fi

if [ ! -f "$INPUT" ]; then
  "$GENERATOR" --count 1000000 --out "$INPUT"
fi

if [ -f "${INPUT}.digest" ]; then
  expected_sha=$(cat "${INPUT}.digest")
  actual_sha=$(shasum -a 256 "$INPUT" | awk '{print $1}')
  if [ "$expected_sha" != "$actual_sha" ]; then
    echo "golden input digest mismatch" >&2
    exit 1
  fi
fi
