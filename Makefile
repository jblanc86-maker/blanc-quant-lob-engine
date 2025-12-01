# Minimal convenience Makefile for Blanc LOB Engine
# Usage:
#   make               # build (Release)
#   make run           # run with defaults
#   make bench         # run + emit artifacts (bench.jsonl, metrics.prom, tarball)
#   make clean         # remove build/
#
# Vars you can override: DATA=..., BUILD_TYPE=Debug, JOBS=8, GAP_PPM=0, BURST_MS=0

BUILD        ?= build
BUILD_TYPE   ?= Release
GENERATOR    ?= Ninja
JOBS         ?= $(shell sysctl -n hw.ncpu 2>/dev/null || nproc)
DATA         ?= data/golden/itch_1m.bin
GAP_PPM      ?= 0
BURST_MS     ?= 0
CPU_PIN      ?=
CPU_PIN_ARG  := $(if $(CPU_PIN),--cpu-pin $(CPU_PIN),)
BIN          := $(BUILD)/bin/replay
ART_DIR      := artifacts
TARBALL      := $(ART_DIR)/blanc-lob-engine-rc.tar.gz

.DEFAULT_GOAL := build

.PHONY: configure
configure:
	@cmake -S . -B $(BUILD) -G $(GENERATOR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure
	@cmake --build $(BUILD) -j $(JOBS)

run: build
	@$(BIN) --input $(DATA) --gap-ppm $(GAP_PPM) --burst-ms $(BURST_MS) $(CPU_PIN_ARG)

bench: build
	@mkdir -p $(ART_DIR)
	# 1) Run with current knobs (deterministic input ⇒ deterministic digest)
	@ART_DIR=$(ART_DIR) $(BIN) --input $(DATA) --gap-ppm $(GAP_PPM) --burst-ms $(BURST_MS) $(CPU_PIN_ARG) | tee $(ART_DIR)/run.stdout.txt
	# 2) Optional repo scripts (no-ops if missing)
	@if [ -x scripts/bench.sh ]; then ART_DIR=$(ART_DIR) CPU_PIN=$(CPU_PIN) scripts/bench.sh 9 || true; fi
	@if [ -x scripts/prom_textfile.sh ]; then scripts/prom_textfile.sh $(ART_DIR)/metrics.prom || true; fi
	# 3) Ensure bench.jsonl exists (make a minimal line if your scripts didn’t produce it)
	@if [ ! -s "$(ART_DIR)/bench.jsonl" ]; then \
	  echo '{"ts":"'"$$(date -u +"%Y-%m-%dT%H:%M:%SZ")"'","input":"$(DATA)","gap_ppm":$(GAP_PPM),"burst_ms":$(BURST_MS)}' > $(ART_DIR)/bench.jsonl; \
	fi
	# 4) Bundle a tiny release candidate tarball (binary + artifacts)
	@tmp=$$(mktemp -u /tmp/blanc-lob-rc.XXXXXX.tar.gz); \
	tar -czf $$tmp -C $(BUILD) bin/replay -C $(CURDIR) $(ART_DIR); \
	mv $$tmp $(TARBALL)

clean:
	@rm -rf $(BUILD) $(ART_DIR)

.PHONY: build run bench clean

# Extras from pre-existing Makefile
golden: build
	./build/bin/gen_synth --count 1000000 --out data/golden/itch_1m.bin
	shasum -a 256 data/golden/itch_1m.bin | awk '{print $$1}' > data/golden/itch_1m.digest

verify:
	@./scripts/verify_golden.sh

release: build verify
	@rm -rf artifacts/release && mkdir -p artifacts/release
	@cp -r build/replay build/gen_synth data/golden scripts/verify_golden.sh artifacts/release/
	@tar -C artifacts -czf artifacts/blanc-lob-engine-rc.tar.gz release
	@echo "Release bundle at artifacts/blanc-lob-engine-rc.tar.gz"

# Create a reproducible release package (zip + manifest + rights_manifest)
.PHONY: release-package
release-package: build
	@echo "Creating release package..."
	@mkdir -p $(ART_DIR)/release
	@./scripts/release_package.sh --build-dir $(BUILD) --art-dir $(ART_DIR) --out-dir $(ART_DIR)/release --git-sha $(shell git rev-parse --short HEAD)
	@echo "Release package created under $(ART_DIR)/release"

.PHONY: packaging-test
packaging-test:
	@bash tests/test_release_package.sh
