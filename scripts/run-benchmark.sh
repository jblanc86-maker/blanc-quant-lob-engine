#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
set -euo pipefail

echo "Building..."
cmake -S . -B build -G Ninja >/dev/null
cmake --build build --parallel >/dev/null
echo "Build complete."

BIN_DIR="$(pwd)/build/bin"
DATA_DIR="$(pwd)/data/golden"

mkdir -p "$DATA_DIR"

echo "Running gen_synth..."
"$BIN_DIR/gen_synth" --out "$DATA_DIR/itch_1m.bin"

echo "Running replay..."
"$BIN_DIR/replay" --input "$DATA_DIR/itch_1m.bin"
