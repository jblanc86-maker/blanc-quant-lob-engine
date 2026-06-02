#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
set -euo pipefail

OS_NAME="$(uname -s)"

# Prefer Apple Clang on macOS; avoid clang-cl selection which breaks on Darwin.
if [[ "$OS_NAME" == "Darwin" ]]; then
	CC_BIN="$(xcrun -find clang)"
	CXX_BIN="$(xcrun -find clang++)"
	AR_BIN="$(xcrun -find ar)"
	RANLIB_BIN="$(xcrun -find ranlib)"
	cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_C_COMPILER="${CC_BIN}" \
		-DCMAKE_CXX_COMPILER="${CXX_BIN}" \
		-DCMAKE_AR="${AR_BIN}" \
		-DCMAKE_RANLIB="${RANLIB_BIN}"
else
	# On Linux, defer to environment or system defaults
	cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
fi
cmake --build build -j
ls -lh build/bin || true
