# CodeQL Workflow Configuration Fix

## Problem Summary

The CodeQL/CMake job failed in PR #29 (copilot/update-codeql-workflow-setup) during the configure stage with internal CMake variable errors. The root cause was traced to failed compiler capability probes due to incorrect package installations and hardcoded paths.

## Root Cause Analysis

The failing workflow (ref: d69d2fbdc18e6997bd7ddd364db24deb3599462c) had three critical issues:

### 1. Invalid Package Names
```yaml
sudo apt-get install -y \
  libpcre++-dev \      # ❌ Package doesn't exist in Ubuntu repos
  libgnuregex-dev \    # ❌ Package doesn't exist in Ubuntu repos
  || true
```

These packages don't exist in Ubuntu repositories. The `|| true` allowed the script to continue, but cmake wasn't installed successfully.

### 2. Hardcoded CMake Path
```bash
/usr/bin/cmake -S . -B build ...  # ❌ cmake is at /usr/local/bin/cmake
```

After `apt-get install cmake`, the binary is located at `/usr/local/bin/cmake`, not `/usr/bin/cmake`. This caused the error:
```
/usr/bin/cmake: No such file or directory
```

### 3. Over-Specified Compiler Paths
```bash
export CC=/usr/bin/gcc-13
export CXX=/usr/bin/g++-13
/usr/bin/cmake -DCMAKE_C_COMPILER=/usr/bin/gcc-13 ...
```

While these paths exist, they are unnecessarily specific and less portable than using environment variables with standard compiler names.

## Solution

The **main branch already has the correct configuration** in `.github/workflows/codeql.yml`:

```yaml
- name: Install build deps
  run: |
    sudo apt-get update
    sudo apt-get install -y cmake ninja-build libboost-all-dev nlohmann-json3-dev g++ zip

- name: Build
  run: |
    rm -rf build
    CC=gcc CXX=g++ cmake -S . -B build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_TESTING=OFF \
      -DBUILD_BENCHMARKS=OFF
    cmake --build build --config Release -j $(nproc)
```

### Why This Works

1. **Valid packages only**: All packages exist in Ubuntu repos
2. **Standard cmake command**: Uses `cmake` from PATH (automatically finds `/usr/local/bin/cmake`)
3. **Environment variables**: Uses `CC=gcc CXX=g++` for compiler selection (portable and standard)
4. **No hardcoded paths**: Works across different installations and configurations

## Verification

All tests pass successfully:

```
Test project /home/runner/work/blanc-quant-lob-engine/blanc-quant-lob-engine/build
    Start 1: telemetry_io
1/5 Test #1: telemetry_io .....................   Passed    0.00 sec
    Start 2: golden_state
2/5 Test #2: golden_state .....................   Passed    0.02 sec
    Start 3: book_snapshot
3/5 Test #3: book_snapshot ....................   Passed    0.02 sec
    Start 4: replay_help
4/5 Test #4: replay_help ......................   Passed    0.00 sec
    Start 5: replay_default_run
5/5 Test #5: replay_default_run ...............   Passed    0.02 sec

100% tests passed, 0 tests failed out of 5
```

## Recommendation

- ✅ **Main branch workflow is correct** - no changes needed
- ❌ **Do not merge PR #29** (copilot/update-codeql-workflow-setup) - it introduces the bugs
- ✅ **This PR** (copilot/fix-cmake-configuration-errors) confirms the repository is in the correct state

## CMake Configuration Details

The project uses CMake 3.20+ with the following key features:
- C++20 standard
- Ninja build system
- Optional benchmarks and tests
- Linux-specific hardening in Release builds
- Check for compiler flags before using them

No regex library dependencies are explicitly needed - the project uses standard C++20 features only.
