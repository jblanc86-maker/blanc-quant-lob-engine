#!/usr/bin/env bash
set -euo pipefail

echo "Building..."
cmake -S . -B build -G Ninja >/dev/null
cmake --build build --parallel >/dev/null
echo "Build complete."

cd build/bin

echo "Running gen_synth..."
./gen_synth

echo "Running replay..."
./replay
