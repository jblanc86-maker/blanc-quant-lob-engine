# App Module

This directory contains the supported application-facing evaluation entry points
for the repository. If you are reviewing the harness for the first time, start
here before dropping into the lower-level replay tooling.

## Files

- `main.cpp`: `app_main`, the primary local app entry point.
- `app_config.hpp` / `app_config.cpp`: Configuration class used by all app entry points.
- `policy.yaml`: Example policy file.
- `test_app.cpp`: `test_app`, a supported validation executable for config behavior.
- `bench_app.cpp`: `bench_app`, a supported benchmark executable for config load timing.
- `CMakeLists.txt`: Build instructions for the app targets.

## Build Instructions

From the project root:

```sh
mkdir -p build && cd build
cmake ..
cmake --build . --target app_main
```

To run the primary evaluation app:

```sh
./apps/app_main
```

To run the validation entry point:

```sh
cmake --build . --target test_app && ./apps/test_app
```

To run the benchmark entry point:

```sh
cmake --build . --target bench_app && ./apps/bench_app
```
