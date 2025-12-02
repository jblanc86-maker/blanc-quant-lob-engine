# SPDX-License-Identifier: Apache-2.0
# ---- builder ----
FROM ubuntu:24.04 AS builder
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
	&& apt-get install -y --no-install-recommends cmake ninja-build g++ ca-certificates make \
	libboost-program-options-dev libboost-dev nlohmann-json3-dev \
	&& rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY . .
# Build (Release)
RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
	&& cmake --build build -j

# Optional: quick conformance at build-time (non-fatal)
# RUN ./scripts/verify_golden.sh || true

# ---- runtime ----
FROM ubuntu:24.04 AS runtime
ENV DEBIAN_FRONTEND=noninteractive
# Minimal runtime deps only; no compilers
RUN apt-get update \
	&& apt-get install -y --no-install-recommends ca-certificates \
	&& rm -rf /var/lib/apt/lists/*
WORKDIR /app

# Non-root user
RUN useradd --system --home /app --shell /usr/sbin/nologin appuser \
	&& chown -R appuser:appuser /app
USER appuser

# Copy in only what we need to run
COPY --from=builder /src/build/replay /app/replay
COPY --from=builder /src/data/golden /app/data/golden
COPY --from=builder /src/scripts/verify_golden.sh /app/scripts/verify_golden.sh

# Default: show determinism / basic run
CMD ["/app/replay","--input","/app/data/golden/itch_1m.bin"]
