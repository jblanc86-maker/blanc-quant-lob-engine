# --- Builder ---
FROM ubuntu:24.04 AS build
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /work

COPY scripts/ scripts/
RUN chmod +x scripts/*.sh && ./scripts/setup-packages.sh
COPY . .
RUN ./scripts/build.sh && ./scripts/post-build.sh

# --- Runtime ---
FROM debian:stable-slim AS runtime
WORKDIR /app
COPY --from=build /work/build/bin/replay /app/replay
RUN useradd -m appuser
USER appuser

ENTRYPOINT ["/app/replay"]
CMD ["--help"]
