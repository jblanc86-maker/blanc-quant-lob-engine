# App Module

This directory contains the main application, configuration, policy, test, and benchmark files.

## Files

- `main.cpp`: Entry point for the app.
- `app_config.hpp` / `app_config.cpp`: Configuration class.
- `policy.yaml`: Example policy file.
- `test_app.cpp`: Simple test for config logic.
- `bench_app.cpp`: Simple benchmark for config loading.
- `CMakeLists.txt`: Build instructions for the app.

## Build Instructions

From the project root:

```sh
mkdir -p build && cd build
cmake ..
make app_main
```

To run the app:

```sh
./apps/app_main
```

To run the test:

```sh
make test_app && ./apps/test_app
```

To run the benchmark:

```sh
make bench_app && ./apps/bench_app
```
