#!/usr/bin/env bash
set -euo pipefail
export DEBIAN_FRONTEND=noninteractive

apt-get update
apt-get install -y --no-install-recommends \
  build-essential cmake ninja-build \
  libboost-all-dev ca-certificates git curl \
  && rm -rf /var/lib/apt/lists/*
