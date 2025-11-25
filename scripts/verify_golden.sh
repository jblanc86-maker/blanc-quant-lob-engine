#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
set -euo pipefail
#!/usr/bin/env bash
set -euo pipefail

BIN="./build/bin/replay"
INPUT="data/golden/itch_1m.bin"

echo "Running determinism check..."

line="$($BIN --input $INPUT)"
digest="$(sed -n 's/.*digest_fnv=0x\([0-9a-f]*\).*/\1/p' <<< "$line")"

if [[ "$digest" != "36b7011851960792" ]]; then
  echo "Determinism FAILED: digest $digest != expected"
  exit 1
fi

echo "Determinism PASS"
