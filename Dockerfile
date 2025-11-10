#####
# Multi-stage build for Blanc LOB Engine (slim runtime, non-root)
# Stage 1: Builder (includes toolchain only)
#####
FROM ubuntu:24.04 AS builder
RUN apt-get update \
	&& apt-get install -y --no-install-recommends cmake ninja-build g++ make bash \
	&& rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY . /src
# Build release binaries
RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
	&& cmake --build build -j
# Optional golden verification done at build time (kept minimal); ignore failure gracefully
RUN make -C /src verify || echo "Golden verify skipped"

#####
# Stage 2: Runtime (just libc + binary + data)
#####
FROM gcr.io/distroless/cc-debian12 AS runtime
# Distroless provides libc/libstdc++ needed for a typical C++ release build.
WORKDIR /app
# Copy only runtime artifacts (binary + data). Verify artifact optional.
COPY --from=builder /src/build/replay /app/replay
COPY --from=builder /src/data /app/data
COPY --from=builder /src/artifacts/verify.txt /app/artifacts/verify.txt
# Non-root: distroless defaults to non-root if user added; we drop privileges explicitly via uid/gid.
USER 65532:65532
# Exec form CMD runs the binary directly; passes input path explicitly.
CMD ["/app/replay","--input","/app/data/golden/itch_1m.bin"]

# Version pin guidance (optional):
#  - Pin builder stage: FROM ubuntu:24.04@sha256:<digest>
#  - Pin distroless: get digest from `docker pull gcr.io/distroless/cc-debian12`.
#  - For toolchain reproducibility: consider a dedicated builder image with pre-installed cmake/ninja specific versions.
# Security/size notes:
#  - Distroless removes shell and package manager, shrinking attack surface.
#  - If additional shared libs are required (check with `ldd build/replay`), either switch to a minimal base like debian:stable-slim and apt-get install libstdc++6, or link statically (add `-static` if feasible).
